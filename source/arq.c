#include "arq.h"
#include "arq_symbols.h"
#include "arq_options.h"
#include "arq_queue.h"
#include "arq_cmd.h"
#include "arq_token.h"
#include "arq_conversion.h"
#include "arq_immediate.h"
#include "arq_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

static void error_msg_append_option(Arq_msg *error_msg, Arq_Option const *o) {
        assert(o != NULL);
        if (o->chr != 0) {
                arq_msg_append_cstr(error_msg, "-");
                arq_msg_append_chr(error_msg, o->chr);
                arq_msg_append_cstr(error_msg, " ");
        }
        if (strlen(o->name) != 0) {
                arq_msg_append_cstr(error_msg, "--");
                arq_msg_append_cstr(error_msg, o->name);
                arq_msg_append_cstr(error_msg, " ");
        }
        if (strlen(o->arguments ) != 0) {
                arq_msg_append_cstr(error_msg, o->arguments);
        }
        arq_msg_append_lf(error_msg);
}

static void error_msg_insert_cmd_line(Arq_msg *m, uint32_t line_nr, Arq_Vector *cmd) {
        uint32_t ln_count = 0;
        uint32_t a_idx = 0; 
        for (uint32_t i = 0; i < m->size; i++) {
                if (m->at[i] == '\n') {
                        ln_count++;
                        if (line_nr == ln_count) {
                                a_idx = i + 1;
                        }
                }
        }

        uint32_t const b_idx = m->size;
        for (uint32_t i = 0; i <= cmd->idx; i++) {
                // render argv to calculate argv_len 
                if (cmd->at[i].id == ARQ_CMD_SHORT_OPTION) {
                        arq_msg_append_chr(m, '-');
                        arq_msg_append_chr(m, cmd->at[i].at[0]);
                        arq_msg_append_chr(m, '_');
                } else if (cmd->at[i].id == ARQ_CMD_LONG_OPTION) {
                        arq_msg_append_nchr(m, '-', 2);
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, '_');
                } else if (cmd->at[i].id == ARQ_CMD_END_OF_LINE) {
                } else {
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, '_');
                }
        }
        arq_msg_append_lf(m);
        uint32_t const c_idx = m->size;
        uint32_t const argv_len = c_idx - b_idx;

        uint32_t const shift_right = b_idx - a_idx;
        for (uint32_t i = 0; i < shift_right; i++) {
                uint32_t const idx = b_idx - 1 - i;
                m->at[idx + argv_len] = m->at[idx];
        }

        uint32_t const d_idx = m->size;
        for (uint32_t i = 0; i <= cmd->idx; i++) {
                // render argv once more for moving argv
                if (cmd->at[i].id == ARQ_CMD_SHORT_OPTION) {
                        arq_msg_append_chr(m, '-');
                        arq_msg_append_chr(m, cmd->at[i].at[0]);
                        arq_msg_append_chr(m, ' ');
                } else if (cmd->at[i].id == ARQ_CMD_LONG_OPTION) {
                        arq_msg_append_nchr(m, '-', 2);
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, ' ');
                } else if (cmd->at[i].id == ARQ_CMD_END_OF_LINE) {
                } else {
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, ' ');
                }
        }
        arq_msg_append_lf(m);

        {
                // moving rendered argv 
                for (uint32_t i = 0; i < argv_len; i++) {
                        m->at[i + a_idx] =  m->at[i + d_idx];
                }
                //delete argv 
                m->size = d_idx;
        }
}

static void output_option_verify_failure(Arq_msg *error_msg, char *arena_buffer) {
        arq_msg_format(error_msg);

        for (uint32_t i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void output_cmd_line_option_failure(Arq_msg *error_msg, char *arena_buffer) {
        arq_msg_format(error_msg);

        for (uint32_t i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void output_cmd_line_conversion_failure(Arq_msg *error_msg, Arq_Option const *o, char *arena_buffer) {
        error_msg_append_option(error_msg, o);

        arq_msg_format(error_msg);

        for (uint32_t i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void call_back_function(Arq_Option const *options, Arq_List const *option_list, Arq_Queue *queue) {
        Arq_Option const *option = &options[option_list->row];
        option->fn(queue);
        assert(queue->read_idx == queue->write_idx && "Queue is not empty, there are still arguments in the queue!");
        arq_queue_clear(queue);
}

uint32_t arq_fn(
        int argc, char **argv, 
        char *arena_buffer, uint32_t const buffer_size,
        Arq_Option const *options, uint32_t const num_of_options
) {
        log_memory("Buffer capacity %d byte", buffer_size);
        Arq_Arena *arena = arq_arena_init(arena_buffer, buffer_size);
        log_memory("Arena head %d byte, capacity %d byte,                arena.size = %4d byte", 
                (int)offsetof(Arq_Arena,at),
                arena->SIZE, 
                arena->size
        );

        Arq_List *option_list = (Arq_List *)arq_arena_malloc(arena, offsetof(Arq_List, at) + num_of_options * sizeof(Arq_OptVector *));
        option_list->num_of_Vec = 0;
        option_list->row = 0;

        log_memory("Options list        %2d + %2d pVect * %2d = %3d byte => arena.size = %4d byte",
                (int)offsetof(Arq_List,at),
                num_of_options,
                (int)sizeof(option_list->at),
                (int)offsetof(Arq_List,at) + (int)(num_of_options * sizeof(option_list->at)),
                arena->size
        );

        Arq_msg error_msg;
        {
                uint32_t SIZE_OF_ERROR_MSG;
                uint32_t const shrink = arena->size;
                char *chr = arq_arena_malloc_rest(arena, 0, sizeof(char), &SIZE_OF_ERROR_MSG);
                arena->size = shrink;
                Arq_msg e_msg = {
                        .SIZE = SIZE_OF_ERROR_MSG,
                        .size = 0,
                        .at = chr,
                };
                memcpy(&error_msg, &e_msg, sizeof(Arq_msg));
        }

        // Arq_OptVector
        for (uint32_t i = 0; i < num_of_options; i++) {
                uint32_t NUM_OF_TOKEN;
                uint32_t const shrink = arena->size;
                Arq_OptVector *v = arq_arena_malloc_rest(arena, offsetof(Arq_OptVector, at), sizeof(Arq_Token), &NUM_OF_TOKEN);
                arena->size = shrink;

                arq_option_tokenize(&options[i], v, NUM_OF_TOKEN);
                Arq_OptVector *vb = arq_arena_malloc(arena,  offsetof(Arq_OptVector, at) + v->num_of_token * sizeof(Arq_Token));
                assert(v == vb);
                log_memory("      Option vector %2d + %2d token * %2d = %3d byte => arena.size = %4d byte", 
                        (int)offsetof(Arq_Token, at),
                        vb->num_of_token,
                        (int)sizeof(vb->at[0]),
                        (int)offsetof(Arq_Token, at) + (int)(vb->num_of_token * sizeof(vb->at[0])),
                        arena->size
                );

                log_tokenizer_option(v, i);

                uint32_to ups = arq_option_verify_vector(v, &error_msg);
                if (ups.error) {
                        error_msg_append_option(&error_msg, &options[i]);
                        uint32_t const n = arq_option_parameter_idx(&options[i]) + ups.u32;
                        arq_msg_append_nchr(&error_msg, ' ', n);
                        arq_msg_append_cstr(&error_msg, "^\n");
                        output_option_verify_failure(&error_msg, arena_buffer);
                        return error_msg.size;
                }

                assert(option_list->num_of_Vec < num_of_options);
                option_list->at[option_list->num_of_Vec++] = v;

        }

///////////////////////////////////////////////////////////////////////////////

        Arq_Vector *cmd;
        {
                uint32_t NUM_OF_TOKEN;
                uint32_t const shrink = arena->size;
                cmd = arq_arena_malloc_rest(arena, offsetof(Arq_Vector, at), sizeof(Arq_Token), &NUM_OF_TOKEN);
                arena->size = shrink;
                arq_cmd_tokenize(argc, argv, cmd, NUM_OF_TOKEN);
                Arq_Vector *cmd_b = arq_arena_malloc(arena,  offsetof(Arq_Vector, at) + cmd->num_of_token * sizeof(Arq_Token));
                assert(cmd == cmd_b);
                log_memory("Command line vector %2d + %2d token * %2d = %3d byte => arena.size = %4d byte", 
                        (int)offsetof(Arq_Vector, at),
                        cmd->num_of_token, 
                        (int)sizeof(cmd->at[0]), 
                        (int)offsetof(Arq_Vector, at) + (int)(cmd->num_of_token * sizeof(cmd->at[0])), 
                        arena->size
                );
        }
        log_tokenizer_cmd_line(cmd);

///////////////////////////////////////////////////////////////////////////////
        log_int_banner("interpreter");
        Arq_Queue *queue = arq_queue_malloc(arena);
        log_memory("Argument queue      %2d + %2d argum * %2d = %3d byte => arena.size = %4d byte", 
                (int)offsetof(Arq_Queue, at),
                queue->NUM_OF_ARGUMENTS,
                (int)sizeof(queue->at[0]),
                (int)offsetof(Arq_Queue, at) + (int)(queue->NUM_OF_ARGUMENTS * sizeof(queue->at[0])),
                arena->size
        );
        log_memory("%d arguments fit in the queue.\n", queue->NUM_OF_ARGUMENTS);

        while(arq_imm_cmd_has_token_left(cmd)) {
                log_int_ln();
                Arq_OptVector *opt = NULL;
                if (arq_imm_cmd_is_long_option(cmd)) {
                        log_int_token(ARQ_CMD_LONG_OPTION);
                        opt = arq_imm_get_long(option_list, options, cmd, &error_msg);
                        if (opt == NULL) {
                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                output_cmd_line_option_failure(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_cmd_is_short_option(cmd)) {
                        log_int_token(ARQ_CMD_SHORT_OPTION);
                        opt = arq_imm_get_short(option_list, options, cmd, &error_msg);
                        if (opt == NULL) {
                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                output_cmd_line_option_failure(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_end_of_line(cmd)) {
                        log_int_token(ARQ_CMD_END_OF_LINE);
                        arena_buffer[0] = 0;
                        return 0;
                } else {
                        arq_imm_cmd_not_a_option(cmd, &error_msg);
                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                        output_cmd_line_option_failure(&error_msg, arena_buffer);
                        return error_msg.size;
                }
                (void)arq_imm_L_parenthesis(opt);
                while (true) {
                        if (arq_imm_type(opt, ARQ_OPT_UINT32_T)) {
                                log_int_token_indent(ARQ_OPT_UINT32_T);
                                (void)arq_imm_not_identifier(opt);
                                uint32_to num;
                                if (arq_imm_equal(opt)) {
                                        num = arq_imm_default_uint32_t(opt);
                                        if (arq_imm_optional_argument_uint32_t(cmd, &num, &error_msg)) {
                                                // overflow
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                } else {
                                        num = arq_imm_argument_uint32_t(cmd, &error_msg);
                                        if (num.error) { 
                                                // wasn't an uint32_t number or overflow
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                }
                                arq_push_uint32_t(queue, num.u32);
                                log_inta("u32 %d", num.u32);
                                if (arq_imm_comma(opt)) continue;
                                goto terminator;
                        }

                        if (arq_imm_type(opt, ARQ_OPT_CSTR_T)) {
                                log_int_token_indent(ARQ_OPT_CSTR_T);
                                (void)arq_imm_not_identifier(opt);
                                char const *cstr; 
                                if (arq_imm_equal(opt)) {
                                        if (arq_imm_cmd_is_dashdash(cmd, opt)) {
                                                cstr = arq_imm_argument_csrt_t(cmd, &error_msg);
                                                if (cstr == NULL) {
                                                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                        arq_msg_append_cstr(&error_msg, "'--' allows you to set an argument that looks like an option -- --hello\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' alone isn't enough if you want '--' as an argument then do -- --\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' undoes optional behavior in case of an cstr_t = NULL\n");
                                                        output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer);
                                                        return error_msg.size;
                                                }
                                        } else {
                                                // For a short option with an optional cstr_t as an argument.
                                                // It is not always possible to include the argument immediately after the option.
                                                // This is the case whether the next character is a bundled option or a character from a cstr_t.
                                                // If the next character is a number, then it is an argument => here is it possible.
                                                // {'S', "cstring", fn_cstring, &ctx, "cstr_t = NULL"},
                                                // failure: -abcShello    => the 'h' is interpreted as short option part of the bundle (no space) thats why failure
                                                // ok:      -abcS hello   => is string token fine
                                                // ok:      -abcS69       => 69 is a number fine can't be a short option
                                                cstr = arq_imm_default_cstr_t(opt);
                                                (void)arq_imm_optional_argument_cstr_t(cmd, &cstr);
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                } else {
                                        // A short option with a mandatory argument allows the argument to be included immediately after the option.
                                        // However, this short option must be the last option in a bundle of options.
                                        // {'S', "cstring", fn_cstring, &ctx, "cstr_t"},
                                        // 'hello' is the argument
                                        // ok: -abcShello    => 'hello' is the argument
                                        // ok: -abcS hello   => 'hello' is the argument
                                        // ok: -abcS--hello  => '--hello' is the argument
                                        // ok: -abcS-hello   => '-hello' is the argument
                                        cstr = arq_imm_argument_csrt_t(cmd, &error_msg);
                                        if (cstr == NULL) {
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                }
                                if (arq_imm_comma(opt)) continue;
                                goto terminator;
                        }
terminator:
                        if (arq_imm_R_parenthesis(opt)) {
                                log_int_comment("call_back_function");
                                call_back_function(options, option_list, queue);
                                break;
                        }
                        assert(false);
                } // while 
        } // while
        arena_buffer[0] = 0;
        return 0;
}

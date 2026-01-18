#include "arq_arena.h"
#include "arq_lexer.h"
#include "arq_msg.h"
#include "arq_queue.h"
#include "arq_immediate.h"
#include "arq_log.h"
#include "arq_bool.h"
#include "arq_token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

static void error_msg_insert_cmd_line(Arq_msg *m, uint32_t line_nr, Arq_LexerCmd *cmd) {
        Arq_Token const token = cmd->lexer.token;
        uint32_t A_IDX, B_IDX, C_IDX, D_IDX, ARGV_LEN; 
        uint32_t ln_count = 0;
        uint32_t i;
        log_int_token(cmd->lexer.token.id);
        A_IDX = 0;
        for (i = 0; i < m->size; i++) {
                if (m->at[i] == '\n') {
                        ln_count++;
                        if (line_nr == ln_count) {
                                A_IDX = i + 1;
                        }
                }
        }

        B_IDX = m->size;
        cmd->argIdx = 0;
        cmd->lexer = arq_lexer_create();
        arq_lexer_next_cmd_token(cmd);
        while(true) {
                /* render argv to calculate argv_len */
                if (cmd->lexer.token.id == ARQ_CMD_SHORT_OPTION) {
                        uint32_t const x = cmd->lexer.token.at[0] == '-' ? 1 : 0; /* because of short option bundeling  */
                        arq_msg_append_chr(m, cmd->lexer.token.at[0]);
                        arq_msg_append_nchr(m, cmd->lexer.token.at[1], x);
                        arq_msg_append_chr(m, '_');
                } else if (cmd->lexer.token.id == ARQ_CMD_LONG_OPTION) {
                        /* arq_msg_append_nchr(m, '-', 2); */
                        arq_msg_append_cstr(m, cmd->lexer.token.at);
                        arq_msg_append_chr(m, '_');
                } else if (cmd->lexer.token.id == ARQ_NO_TOKEN) {
                } else {
                        arq_msg_append_cstr(m, cmd->lexer.token.at);
                        arq_msg_append_chr(m, '_');
                }
                if (cmd->lexer.token.at == token.at) {
                        break;
                }
                arq_lexer_next_cmd_token(cmd);
        }
        arq_msg_append_lf(m);
        C_IDX = m->size;
        ARGV_LEN = C_IDX - B_IDX;
        {
                uint32_t const shift_right = B_IDX - A_IDX;
                for (i = 0; i < shift_right; i++) {
                        uint32_t const idx = B_IDX - 1 - i;
                        m->at[idx + ARGV_LEN] = m->at[idx];
                }
        }

        D_IDX = m->size;
        cmd->argIdx = 0;
        cmd->lexer = arq_lexer_create();
        arq_lexer_next_cmd_token(cmd);
        while(true) {
                /* render argv once more for moving argv */
                if (cmd->lexer.token.id == ARQ_CMD_SHORT_OPTION) {
                        uint32_t const x = cmd->lexer.token.at[0] == '-' ? 1 : 0; /* because of short option bundeling  */
                        arq_msg_append_chr(m, cmd->lexer.token.at[0]);
                        arq_msg_append_nchr(m, cmd->lexer.token.at[1], x);
                        arq_msg_append_chr(m, ' ');
                } else if (cmd->lexer.token.id == ARQ_CMD_LONG_OPTION) {
                        /* arq_msg_append_nchr(m, '-', 2); */
                        arq_msg_append_cstr(m, cmd->lexer.token.at);
                        arq_msg_append_chr(m, ' ');
                } else if (cmd->lexer.token.id == ARQ_NO_TOKEN) {
                } else {
                        arq_msg_append_cstr(m, cmd->lexer.token.at);
                        arq_msg_append_chr(m, ' ');
                }
                if (cmd->lexer.token.at == token.at) {
                        break;
                }
                arq_lexer_next_cmd_token(cmd);
        }
        arq_msg_append_lf(m);

        {
                /* moving rendered argv */
                for (i = 0; i < ARGV_LEN; i++) {
                        m->at[i + A_IDX] =  m->at[i + D_IDX];
                }
                /* delete argv */
                m->size = D_IDX;
        }
}

static void output_error_msg(Arq_msg *error_msg, char *arena_buffer) {
        uint32_t i;
        arq_msg_format(error_msg);
        for (i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void call_back_function(Arq_Option const *options, uint32_t option_idx, Arq_Queue *queue) {
        Arq_Option const *option = &options[option_idx];
        option->fn(queue);
        assert(queue->read_idx == queue->write_idx && "Queue is not empty, there are still arguments in the queue!");
        arq_queue_clear(queue);
}

static uint32_t arq_option_parameter_idx(Arq_Option const *option) {
        uint32_t STRLEN;
        uint32_t result = 0;
        if (option->chr != 0) {
                result += 3;
        }
        STRLEN = strlen(option->name);
        if (STRLEN > 0) {
                result += STRLEN + 3;
        }
        return result;
}

uint32_t arq_verify(
        char *arena_buffer, uint32_t const buffer_size,
        Arq_Option const *options, uint32_t const num_of_options
) {
        Arq_msg error_msg;
        uint32_t i;
        Arq_Arena *arena;
        (void) buffer_size;

        arena = arq_arena_init(arena_buffer, buffer_size);

        {
                uint32_t SIZE_OF_ERROR_MSG;
                uint32_t const shrink = arena->size;
                char *chr = (char *)arq_arena_malloc_rest(arena, 0, sizeof(char), &SIZE_OF_ERROR_MSG);
                arena->size = shrink;
                error_msg.SIZE = SIZE_OF_ERROR_MSG;
                error_msg.size = 0;
                error_msg.at = chr;
        }

        for (i = 0; i < num_of_options; i++) {
                bool for_loop_continue = false;
                Arq_LexerOpt opt = arq_lexerOpt_create();
                opt.lexer.at = options[i].arguments;
                opt.lexer.SIZE = strlen(options[i].arguments);
                opt.lexer.cursor_idx = 0;
                arq_lexer_next_opt_token(&opt);
                arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                arq_msg_append_cstr(&error_msg, "' missing open parenthesis '('\n");
                if (arq_imm(ARQ_OP_L_PARENTHESIS, &opt)) {
                        if (arq_imm(ARQ_OP_R_PARENTHESIS, &opt)) {
                                if (arq_imm_noToken(&opt.lexer.token)) {
                                        continue;
                                } 
                                arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                                arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                                arq_msg_append_cstr(&error_msg, "' after ')' no tokens allowed!\n");
                                goto error;
                        }
                        do {
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                                arq_fn_imm_literal_error LITERAL_ERROR = NULL;
                                if      (arq_imm(ARQ_TYPE_UINT,  &opt)) LITERAL_ERROR = arq_imm_literal_uint_error;
                                else if (arq_imm(ARQ_TYPE_INT,   &opt)) LITERAL_ERROR = arq_imm_literal_int_error;
                                else if (arq_imm(ARQ_TYPE_FLOAT, &opt)) LITERAL_ERROR = arq_imm_literal_float_error;
                                else if (arq_imm(ARQ_TYPE_CSTR,  &opt)) LITERAL_ERROR = arq_imm_literal_NULL_error;
                                else {
                                        arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                                        arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                                        arq_msg_append_cstr(&error_msg, "' is not a type\n");
                                        goto error;
                                }

                                if (arq_imm_not_identifier(&opt)) {
                                        arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                                        arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                                        arq_msg_append_cstr(&error_msg, "' is not a parameter name\n");
                                        goto error;
                                } else {
                                        arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                                        arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                                        arq_msg_append_cstr(&error_msg, "' but expected '=' or '[]' or ',' or ')'\n");
                                }
                                if (arq_imm(ARQ_OP_EQ, &opt)) {
                                        if (LITERAL_ERROR(&opt, &error_msg)) {
                                                goto error;
                                        }
                                        arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                                        arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                                        arq_msg_append_cstr(&error_msg, "' but expected ',' or ')'\n");
                                } else if (arq_imm(ARQ_OP_ARRAY, &opt)) {
                                        arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                                        arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                                        arq_msg_append_cstr(&error_msg, "' but expected ',' or ')'\n");
                                }
                                if (arq_imm(ARQ_OP_COMMA, &opt)) {
                                        continue;
                                }
                                if (arq_imm(ARQ_OP_R_PARENTHESIS, &opt)) {
                                        arq_msg_set_cstr(&error_msg, OPTION_FAILURE);
                                        arq_msg_append_str(&error_msg, opt.lexer.token.at, opt.lexer.token.size);
                                        arq_msg_append_cstr(&error_msg, "' after ')' no tokens allowed!\n");
                                        if (arq_imm_noToken(&opt.lexer.token)) {
                                                for_loop_continue = true; 
                                                break;
                                        } 
                                }
                                goto error;
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        } while (opt.lexer.token.id != ARQ_NO_TOKEN);
                        if (for_loop_continue) {
                                continue;
                        }
                }
error:
                {
                        uint32_t n;
                        uint_o ups; 
                        ups.error = true; 
                        ups.u = opt.lexer.cursor_idx - opt.lexer.token.size;
                        n = arq_option_parameter_idx(&options[i]) + ups.u;
                        error_msg_append_option(&error_msg, &options[i]);
                        arq_msg_append_nchr(&error_msg, ' ', n);
                        arq_msg_append_cstr(&error_msg, "^\n");
                        output_error_msg(&error_msg, arena_buffer);
                        return error_msg.size;
                }

        } /* for loop */
        /* assert(false); */
        return 0;
}

uint32_t arq_fn(
        int argc, char **argv, 
        char *arena_buffer, uint32_t const buffer_size,
        Arq_Option const *options, uint32_t const num_of_options
) {
        Arq_LexerCmd cmd = arq_lexerCmd_create(argc, argv);
        Arq_LexerOpt opt;
        Arq_Arena *arena;
        Arq_msg error_msg;
        Arq_Queue *queue;

        log_memory( ("size of arq_int %ld bit\n", 8 * sizeof(((uint_o *)0)->u) ));

        log_options_tokens(options, num_of_options);
        log_cmd_tokens(argc, argv);

        log_memory(("------- memory usage in byte --------"));
        log_memory(("%11shead %2scapacity %6srest", "", "", ""));
        log_memory(("Buffer    %5d %10d %10s", 0, buffer_size, "-"));
        arena = arq_arena_init(arena_buffer, buffer_size);
        log_memory(("Arena     %5d %10d %10s", (int)offsetof(Arq_Arena, at), arena->SIZE, "-"));

        {
                uint32_t SIZE_OF_ERROR_MSG = 500;
                error_msg.at = (char *)arq_arena_malloc(arena, SIZE_OF_ERROR_MSG);
                error_msg.SIZE = SIZE_OF_ERROR_MSG;
                error_msg.size = 0;
                log_memory(("error_msg %5d %10d %10d", 0, error_msg.SIZE, (int)(arena->SIZE - arena->size)));
        }

        log_int_banner("interpreter");

        queue = arq_queue_malloc(arena);

        log_memory(("Arq_Queue %5d %10d %10d", 
                (int)offsetof(Arq_Queue, at), 
                (int)(queue->NUM_OF_ARGUMENTS * sizeof(queue->at[0])),
                (int)(arena->SIZE - arena->size))
                /*(int)(arena->SIZE - queue->NUM_OF_ARGUMENTS * sizeof(queue->at[0])))*/

        );
        log_memory(("\n%d arguments fit in the queue.\n", queue->NUM_OF_ARGUMENTS));


        arq_lexer_next_cmd_token(&cmd);
        while(arq_imm_cmd_has_token_left(&cmd)) {
                /* Arq_OptVector *opt = NULL; */
                log_int_ln();
                if (arq_imm_cmd_is_long_option(&cmd)) {
                        log_int_token(ARQ_CMD_LONG_OPTION);
                        opt = arq_imm_get_long(options, num_of_options, &cmd, &error_msg);
                        if (opt.lexer.at == NULL) {
                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                output_error_msg(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_cmd_is_short_option(&cmd)) {
                        log_int_token(ARQ_CMD_SHORT_OPTION);
                        opt = arq_imm_get_short(options, num_of_options, &cmd, &error_msg);
                        if (opt.lexer.at == NULL) {
                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                output_error_msg(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_end_of_line(&cmd)) {
                        log_int_token(ARQ_NO_TOKEN);
                        arena_buffer[0] = 0;
                        return 0;
                } else {
                        arq_imm_cmd_not_a_option(&cmd, &error_msg);
                        error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                        output_error_msg(&error_msg, arena_buffer);
                        return error_msg.size;
                }
                arq_lexer_next_opt_token(&opt);
                (void)arq_imm(ARQ_OP_L_PARENTHESIS, &opt);
                while (true) {
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm(ARQ_TYPE_UINT, &opt)) {
                                log_int_token_indent(ARQ_TYPE_UINT);
                                (void)arq_imm_not_identifier(&opt);
                                if (arq_imm(ARQ_OP_EQ, &opt)) {
                                        uint_o num = arq_imm_default_uint(&opt);
                                        if (arq_imm_optional_argument_uint(&cmd, &num, &error_msg)) {
                                                /* overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                error_msg_append_option(&error_msg, &options[opt.idx]);
                                                output_error_msg(&error_msg, arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_uint(queue, num.u);
                                        log_inta(("u32 %u", num.u));
                                } else if (arq_imm(ARQ_OP_ARRAY, &opt)) {
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (arq_imm_is_p_dec(&cmd) || arq_imm_is_hex(&cmd)) {
                                                uint_o num = {0};
                                                if (arq_imm_optional_argument_uint(&cmd, &num, &error_msg)) {
                                                        /* overflow */
                                                        error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                        error_msg_append_option(&error_msg, &options[opt.idx]);
                                                        output_error_msg(&error_msg, arena_buffer);
                                                        return error_msg.size;
                                                }
                                                *array_size += 1;
                                                arq_push_uint(queue, num.u);
                                                log_inta(("u32 %u", num.u));
                                        }
                                } else {
                                        uint_o const num = arq_imm_argument_uint(&cmd, &error_msg);
                                        if (num.error) { 
                                                /* wasn't an uint32_t number or overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                error_msg_append_option(&error_msg, &options[opt.idx]);
                                                output_error_msg(&error_msg, arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_uint(queue, num.u);
                                        log_inta(("u32 %u", num.u));
                                }
                                if (arq_imm(ARQ_OP_COMMA, &opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm(ARQ_TYPE_INT, &opt)) {
                                log_int_token_indent(ARQ_TYPE_INT);
                                (void)arq_imm_not_identifier(&opt);
                                if (arq_imm(ARQ_OP_EQ, &opt)) {
                                        int_o num = arq_imm_default_int(&opt);
                                        if (arq_imm_optional_argument_int(&cmd, &num, &error_msg)) {
                                                /* overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                error_msg_append_option(&error_msg, &options[opt.idx]);
                                                output_error_msg(&error_msg, arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_int(queue, num.i);
                                        log_inta(("i32 %d", num.i));
                                } else if (arq_imm(ARQ_OP_ARRAY, &opt)) {
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (arq_imm_is_p_dec(&cmd) || arq_imm_is_n_dec(&cmd) || arq_imm_is_hex(&cmd)) {
                                                int_o num = {0};
                                                if (arq_imm_optional_argument_int(&cmd, &num, &error_msg)) {
                                                        /* overflow */
                                                        error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                        error_msg_append_option(&error_msg, &options[opt.idx]);
                                                        output_error_msg(&error_msg, arena_buffer);
                                                        return error_msg.size;
                                                }
                                                *array_size += 1;
                                                arq_push_int(queue, num.i);
                                                log_inta(("i32 %d", num.i));
                                        }
                                } else {
                                        int_o const num = arq_imm_argument_int(&cmd, &error_msg);
                                        if (num.error) { 
                                                /* wasn't an int32_t number or overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                error_msg_append_option(&error_msg, &options[opt.idx]);
                                                output_error_msg(&error_msg, arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_int(queue, num.i);
                                        log_inta(("i32 %d", num.i));
                                }
                                if (arq_imm(ARQ_OP_COMMA, &opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm(ARQ_TYPE_FLOAT, &opt)) {
                                log_int_token_indent(ARQ_TYPE_FLOAT);
                                (void)arq_imm_not_identifier(&opt);
                                if (arq_imm(ARQ_OP_EQ, &opt)) {
                                        float_o num = arq_imm_default_float(&opt);
                                        if (arq_imm_optional_argument_float(&cmd, &num, &error_msg)) {
                                                /* in the moment there is no error */
                                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                error_msg_append_option(&error_msg, &options[opt.idx]);
                                                output_error_msg(&error_msg, arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_float(queue, num.f);
                                        log_inta(("float %f", num.f));
                                } else if (arq_imm(ARQ_OP_ARRAY, &opt)) {
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (arq_imm_is_hexFloat(&cmd)) {
                                                float_o num = {0};
                                                if (arq_imm_optional_argument_float(&cmd, &num, &error_msg)) {
                                                        /* in the moment there is no error */
                                                        error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                        error_msg_append_option(&error_msg, &options[opt.idx]);
                                                        output_error_msg(&error_msg, arena_buffer);
                                                        return error_msg.size;
                                                }
                                                *array_size += 1;
                                                arq_push_float(queue, num.f);
                                                log_inta(("float %f", num.f));
                                        }
                                } else {
                                        float_o const num = arq_imm_argument_float(&cmd, &error_msg);
                                        if (num.error) { 
                                                /* wasn't an float number */
                                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                error_msg_append_option(&error_msg, &options[opt.idx]);
                                                output_error_msg(&error_msg, arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_float(queue, num.f);
                                        log_inta(("float %f", num.f));
                                }
                                if (arq_imm(ARQ_OP_COMMA, &opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm(ARQ_TYPE_CSTR, &opt)) {
                                char const *cstr; 
                                log_int_token_indent(ARQ_TYPE_CSTR);
                                (void)arq_imm_not_identifier(&opt);
                                if (arq_imm(ARQ_OP_EQ, &opt)) {
                                        cstr = arq_imm_default_cstr_t(&opt);
                                        if (arq_imm_cmd_is_dashdash(&cmd)) {
                                                cstr = arq_imm_argument_csrt_t(&cmd, &error_msg);
                                                if (cstr == NULL) {
                                                        error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                        arq_msg_append_cstr(&error_msg, "'--' alone isn't enough if you want '--' as an argument then do -- --\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' allows you to set an argument that looks like an option -- --hello\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' undoes optional behavior in case of an cstr_t = NULL\n");
                                                        error_msg_append_option(&error_msg, &options[opt.idx]);
                                                        output_error_msg(&error_msg, arena_buffer);
                                                        return error_msg.size;
                                                }
                                        } else {
                                                /* For a short option with an optional cstr_t as an argument. */
                                                /* It is not always possible to include the argument immediately after the option. */
                                                /* This is the case whether the next character is a bundled option or a character from a cstr_t. */
                                                /* If the next character is a number, then it is an argument => here is it possible. */
                                                /* {'S', "cstring", fn_cstring, &ctx, "cstr_t = NULL"}, */
                                                /* failure: -abcShello    => the 'h' is interpreted as short option part of the bundle (no space) thats why failure */
                                                /* ok:      -abcS hello   => is string token fine */
                                                /* ok:      -abcS69       => 69 is a number fine can't be a short option */
                                                (void)arq_imm_optional_argument_cstr_t(&cmd, &cstr);
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                } else if (arq_imm(ARQ_OP_ARRAY, &opt)) {
                                        struct {
                                                bool on;
                                                bool edge;
                                        } dashdash = {0};
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (1) {
                                                dashdash.on |= arq_imm_cmd_is_dashdash(&cmd);
                                                if (dashdash.on && !arq_imm_pick_cstr_t(&cmd, &cstr)) { 
                                                        if (dashdash.edge) { 
                                                                break; 
                                                        }
                                                        arq_msg_append_cstr(&error_msg, CMD_LINE_FAILURE);
                                                        error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                        arq_msg_append_cstr(&error_msg, "'--' alone isn't enough if you want '--' as an argument then do -- --\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' allows to set an argument that looks like an option -- --hello\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' switch to positional arguments in case of an cstr_t array\n");
                                                        error_msg_append_option(&error_msg, &options[opt.idx]);
                                                        output_error_msg(&error_msg, arena_buffer);
                                                        return error_msg.size;
                                                } 
                                                if (!dashdash.on && !arq_imm_optional_argument_cstr_t(&cmd, &cstr)) { 
                                                        break;
                                                }
                                                dashdash.edge = dashdash.on;
                                                *array_size += 1;
                                                arq_push_cstr_t(queue, cstr);
                                                log_inta(("cstr %s", cstr));
                                        }
                                } else {
                                        /* A short option with a mandatory argument allows the argument to be included immediately after the option. */
                                        /* However, this short option must be the last option in a bundle of options. */
                                        /* {'S', "cstring", fn_cstring, &ctx, "cstr_t"}, */
                                        /* 'hello' is the argument */
                                        /* ok: -abcShello    => 'hello' is the argument */
                                        /* ok: -abcS hello   => 'hello' is the argument */
                                        /* ok: -abcS--hello  => '--hello' is the argument */
                                        /* ok: -abcS-hello   => '-hello' is the argument */
                                        cstr = arq_imm_argument_csrt_t(&cmd, &error_msg);
                                        if (cstr == NULL) {
                                                error_msg_insert_cmd_line(&error_msg, 1, &cmd);
                                                error_msg_append_option(&error_msg, &options[opt.idx]);
                                                output_error_msg(&error_msg, arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                }
                                if (arq_imm(ARQ_OP_COMMA, &opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
terminator:
                        if (arq_imm(ARQ_OP_R_PARENTHESIS, &opt)) {
                                log_int_comment("call_back_function");
                                call_back_function(options, opt.idx, queue);
                                break;
                        }
                        assert(false);
                } /* while */
        } /* while */
        arena_buffer[0] = 0;
        return 0;
}

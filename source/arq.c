#include "arq.h"
#include "arq_symbols.h"
#include "arq_options.h"
#include "arq_queue.h"
#include "arq_cmd.h"
#include "arq_token.h"
#include "arq_conversion.h"
#include "arq_msg.h"
#include "arq_immediate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

static void add_option_msg(Arq_msg *error_msg, Arq_Option const *o) {
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

static void error_msg_print_buffer(Arq_msg *error_msg, Arq_Option const *o, char *arena_buffer) {
        add_option_msg(error_msg, o);

        arq_msg_format(error_msg);

        for (uint32_t i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void option_error_msg_print_buffer(Arq_msg *error_msg, char *arena_buffer) {
        arq_msg_format(error_msg);

        for (uint32_t i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void print_token(Arq_Token *t) {
        printf("%3d %30s -> ", t->size, symbol_names[t->id]);
        for (uint32_t i = 0; i < t->size; i++) {
                putchar(t->at[i]);
        }
        printf(" %d\n",t->size);
}

static void Error_msg_cmd_failure(Arq_msg *error_msg, char const *cstrA, Arq_Token const token, char const *cstrB) {
        arq_msg_clear(error_msg);
        arq_msg_append_cstr(error_msg, "CMD line failure:\n");
        arq_msg_append_cstr(error_msg, cstrA);
        arq_msg_append_str(error_msg, token.at, token.size);
        arq_msg_append_cstr(error_msg, cstrB);
        arq_msg_append_lf(error_msg);
}

static void call_back_function(Arq_Option const *options, Arq_List const *option_list, Arq_Queue *queue) {
        Arq_Option const *option = &options[option_list->row];
        option->fn(queue);
        assert(queue->read_idx == queue->write_idx && "Queue is not empty, there are still arguments in the queue!");
}

uint32_t arq_fn(
        int argc, char **argv, 
        char *arena_buffer, uint32_t const buffer_size,
        Arq_Option const *options, uint32_t const num_of_options
) {
        Arq_Arena *arena = arq_arena_init(arena_buffer, buffer_size);

        Arq_List *option_list = (Arq_List *)arq_arena_malloc(arena, offsetof(Arq_List, at) + num_of_options * sizeof(Arq_OptVector *));
        option_list->num_of_Vec = 0;
        option_list->row = 0;

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

                printf("Option %d:\n", i);
                for (uint32_t j = 0; j < v->num_of_token; j++) {
                        print_token(&v->at[j]);
                }

                uint32_to ups = arq_option_verify_vector(v, &error_msg);
                if (ups.error) {
                        add_option_msg(&error_msg, &options[i]);
                        uint32_t const n = arq_option_parameter_idx(&options[i]) + ups.u32;
                        arq_msg_append_nchr(&error_msg, ' ', n);
                        arq_msg_append_cstr(&error_msg, "^\n");
                        option_error_msg_print_buffer(&error_msg, arena_buffer);
                        return error_msg.size;
                }

                assert(option_list->num_of_Vec < num_of_options);
                option_list->at[option_list->num_of_Vec++] = v;

        }
        printf("\n");

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
        }
        printf("Command line: %d \n", cmd->num_of_token);
        for (uint32_t i = 0; i < cmd->num_of_token; i++) {
                print_token(&cmd->at[i]);
        }

///////////////////////////////////////////////////////////////////////////////

        printf("\n-------- interpreter --------------\n\n");
        Arq_Queue *queue = arq_queue_malloc(arena);
        printf("Max possible arguments to put in queue %d\n", queue->NUM_OF_ARGUMENTS);

        while(arq_imm_cmd_has_token_left(cmd)) {
                printf("\n");
                Arq_OptVector *opt = NULL;
                if (arq_imm_cmd_is_long_option(cmd)) {
                        printf("ARQ_CMD_LONG_OPTION\n");
                        opt = arq_imm_get_long(option_list, options, cmd);
                        if (opt == NULL) {
                                Error_msg_cmd_failure(&error_msg, "'--", cmd->at[cmd->idx], "' unknown long option ");
                                option_error_msg_print_buffer(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_cmd_is_short_option(cmd)) {
                        printf("ARQ_CMD_SHORT_OPTION\n");
                        opt = arq_imm_get_short(option_list, options, cmd);
                        if (opt == NULL) {
                                Error_msg_cmd_failure(&error_msg, "'-", cmd->at[cmd->idx], "' unknown short option");
                                option_error_msg_print_buffer(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_end_of_line(cmd)) {
                        printf("cmd end!\n");
                        arena_buffer[0] = 0;
                        return 0;
                } else {
                        Error_msg_cmd_failure(&error_msg, "'", cmd->at[cmd->idx], "' is not an option");
                        option_error_msg_print_buffer(&error_msg, arena_buffer);
                        return error_msg.size;
                }
                (void)arq_imm_L_parenthesis(opt);
                while (true) {
                        if (arq_imm_type(opt, ARQ_OPT_UINT32_T)) {
                                printf("ARQ_OPT_UINT32_T\n");
                                (void)arq_imm_not_identifier(opt);
                                uint32_to num;
                                if (arq_imm_equal(opt)) {
                                        num = arq_imm_default_uint32_t(opt);
                                        if (arq_imm_optional_argument_uint32_t(cmd, &num, &error_msg)) {
                                                // overflow
                                                error_msg_print_buffer(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                } else {
                                        num = arq_imm_argument_uint32_t(cmd, &error_msg);
                                        if (num.error) { 
                                                // wasn't an uint32_t number or overflow
                                                arq_msg_insert_ln_argv(&error_msg, 2, argc, argv);
                                                error_msg_print_buffer(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                }
                                arq_push_uint32_t(queue, num.u32);
                                printf("u32 %d\n", num.u32);
                                if (arq_imm_comma(opt)) continue;
                                goto terminator;
                        }

                        if (arq_imm_type(opt, ARQ_OPT_CSTR_T)) {
                                printf("ARQ_OPT_CSTR_T\n");
                                (void)arq_imm_not_identifier(opt);
                                char const *cstr; 
                                if (arq_imm_equal(opt)) {
                                        if (arq_imm_cmd_is_dashdash(cmd, opt)) {
                                                cstr = arq_imm_argument_csrt_t(cmd, &error_msg);
                                                if (cstr == NULL) {
                                                        arq_msg_append_cstr(&error_msg, "'--' allows you to set an argument that looks like an option -- --hello\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' alone isn't enough if you want '--' as an argument then do -- --\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' undoes optional behavior in case of an cstr_t = NULL\n");
                                                        error_msg_print_buffer(&error_msg, &options[option_list->row], arena_buffer);
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
                                                error_msg_print_buffer(&error_msg, &options[option_list->row], arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                }
                                if (arq_imm_comma(opt)) continue;
                                goto terminator;
                        }
terminator:
                        if (arq_imm_R_parenthesis(opt)) {
                                printf("ARQ_OPTION_END\n");
                                call_back_function(options, option_list, queue);
                                break;
                        }
                        assert(false);
                } // while 
        } // while
        arena_buffer[0] = 0;
        return 0;
}

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

char error_buffer[1000];
Arq_msg error_msg = {
        .SIZE = sizeof(error_buffer),
        .size = 0,
        .at = error_buffer,
};

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
                arq_msg_append_cstr(error_msg, "(");
                arq_msg_append_cstr(error_msg, o->arguments);
                arq_msg_append_cstr(error_msg, ")");
        }
        arq_msg_append_lf(error_msg);
}

static void end(Arq_msg *error_msg, Arq_Option const *o) {
        if (o != NULL) {
                add_option_msg(error_msg, o);
        }
        arq_msg_format(error_msg);
        printf("%s", error_msg->at);
        exit(1);
}

static void print_token(Arq_Token *t) {
        printf("%3d %30s -> ", t->size, symbol_names[t->id]);
        for (uint32_t i = 0; i < t->size; i++) {
                putchar(t->at[i]);
        }
        printf("\n");
}

static void Error_msg_cmd_failure(Arq_msg *error_msg, char const *cstrA, Arq_Token const *token, char const *cstrB) {
        arq_msg_append_cstr(error_msg, "CMD line failure:\n");
        arq_msg_append_cstr(error_msg, cstrA);
        arq_msg_append_str(error_msg, token->at, token->size);
        arq_msg_append_cstr(error_msg, cstrB);
        arq_msg_append_lf(error_msg);
}

static void call_option_fn(Arq_Option const * option, Arq_Queue *queue) {
        option->fn(option->context, queue);
}

// jumps over a bundel of short options
// -shello
// s is a short option
// if s take no argument then 'h' and maybe 'ello' are all short options
// if s takes an argument then 'hello' is the argument
//     'next_bundle_idx' over jumps 'hello' because in the vector they all bundled short options
//     'hello' should be a cstring in the vector but it isn't.
//     That's why we have to increment idx for every char (short option) in the bundle 'hello'
#define test
#ifndef test
static void next_bundle_idx(Arq_Vector *v) {
        char const *const begin = v->at[v->idx].at;
        char const *const end = begin + strlen(begin);
        assert(*end == 0);
        char const *x = begin;
        while ((v->idx < v->num_of_token - 1) && (x >= begin) && (x < end)) {
                v->idx++;
                x = v->at[v->idx].at;
        }
}
#endif

void arq_fn(int argc, char **argv, Arq_Arena *arena, Arq_Option const *options, uint32_t const num_of_options) {
        Arq_List *option_list = (Arq_List *)arq_arena_malloc(arena, offsetof(Arq_List, at) + num_of_options * sizeof(Arq_Vector *));
        option_list->num_of_Vec = 0;
        option_list->row = 0;


        for (uint32_t i = 0; i < num_of_options; i++) {
                uint32_t NUM_OF_TOKEN;
                uint32_t const shrink = arena->size;
                Arq_Vector *v = arq_arena_malloc_rest(arena, offsetof(Arq_Vector, at), sizeof(Arq_Token), &NUM_OF_TOKEN);
                arena->size = shrink;

                arq_option_tokenize(&options[i], v, NUM_OF_TOKEN);
                Arq_Vector *vb = arq_arena_malloc(arena,  offsetof(Arq_Vector, at) + v->num_of_token * sizeof(Arq_Token));
                assert(v == vb);

                printf("Option %d:\n", i);
                for (uint32_t j = 0; j < v->num_of_token; j++) {
                        print_token(&v->at[j]);
                }

                uint32_to ups = arq_option_verify_vector(v, &error_msg);
                if (ups.error) {
                        add_option_msg(&error_msg, &options[i]);
                        uint32_t const n = arq_option_parameter_idx(&options[i]) + 1 + ups.u32;
                        arq_msg_append_nchr(&error_msg, ' ', n);
                        arq_msg_append_cstr(&error_msg, "^\n");
                        end(&error_msg, NULL);
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
        // {
        //         uint32_t NUM_OF_TOKEN;
        //         uint32_t const shrink = arena->size;
        //         Arq_Vector *queue = arq_arena_malloc_rest(arena, offsetof(Stack, at), sizeof(Arq_Token), &NUM_OF_TOKEN);
        //         arena->size = shrink;

        //         // arq_stack_push_uint32_t(num.u32);
        // }
        Arq_Queue *queue = arq_queue_malloc(arena);
        printf("Max possible arguments %d\n", queue->NUM_OF_ARGUMENTS);

        while(arq_imm_cmd_has_token_left(cmd)) {
                Arq_Vector *opt = NULL;
                if (arq_imm_cmd_is_long_option(cmd)) {
                        printf("ARQ_CMD_LONG_OPTION\n");
                        opt = arq_imm_get_long(option_list, options, cmd);
                        if (opt == NULL) {
                                Error_msg_cmd_failure(&error_msg, "'--", &cmd->at[cmd->idx], "' unknown long option ");
                                end(&error_msg, NULL);

                        }
                } else if (arq_imm_cmd_is_short_option(cmd)) {
                        printf("ARQ_CMD_SHORT_OPTION\n");
                        opt = arq_imm_get_short(option_list, options, cmd);
                        if (opt == NULL) {
                                Error_msg_cmd_failure(&error_msg, "'-", &cmd->at[cmd->idx], "' unknown short option");
                                end(&error_msg, NULL);
                        }
                } else if (cmd->at[cmd->idx].id == ARQ_END) {
                        printf("cmd end!\n");
                        return;
                } else {
                        Error_msg_cmd_failure(&error_msg, "'", &cmd->at[cmd->idx], "' is not an option");
                        end(&error_msg, NULL);
                }
                arq_imm_next(cmd);
                while (true) {
                        if (arq_imm_type(opt, ARQ_PARA_UINT32_T)) {
                                printf("ARQ_PARA_UINT32_T\n");
                                uint32_to num;
                                if (arq_imm_assignment_equal(opt)) {
                                        num = arq_imm_default_uint32_t(opt);
                                        if (arq_imm_if_uint32_t(cmd, &num, &error_msg)) {
                                                // number conversion failed
                                                end(&error_msg, &options[option_list->row]); 
                                        }
                                } else {
                                        num = arq_imm_take_uint32_t(cmd, &error_msg);
                                        if (num.error) { 
                                                // wasn't an uint32_t or number or conversion failed
                                                end(&error_msg, &options[option_list->row]); 
                                        }
                                }
                                arq_push_uint32_t(queue, num.u32);
                                printf("u32 %d\n", num.u32);
                                continue;
                        }

#ifdef test 
                        if (arq_imm_type(opt, ARQ_PARA_CSTR_T)) {
                                printf("ARQ_PARA_CSTR_T\n");
                                char const *cstr; 
                                if (arq_imm_assignment_equal(opt)) {
                                        // For a short option with an optional cstr_t as an argument.
                                        // It is not always possible to include the argument immediately after the option.
                                        // This is the case whether the next character is a bundled option or a character from a cstr_t.
                                        // If the next character is a number, then it is an argument => here is it possible.
                                        // {'S', "cstring", fn_cstring, &ctx, "cstr_t = NULL"},
                                        // failure: -abcShello    => the 'h' would is interpreted as short option part of the bundle (no space) thats why failure
                                        // ok:      -abcS hello   => is string token fine
                                        // ok:      -abcS69       => 69 is a number fine can't be a short option
                                        cstr = arq_imm_default_cstr_t(opt);
                                        arq_imm_if_cstr_t(cmd, &cstr);
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
                                        cstr = arq_imm_take_csrt_t(cmd, &error_msg);
                                        if (cstr == NULL) {
                                                end(&error_msg, &options[option_list->row]);
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                }
                                continue;
                        }
#else
                        if (arq_imm_type(opt, ARQ_PARA_CSTR_T)) {
                                printf("ARQ_PARA_CSTR_T\n");
                                if (arq_imm_assignment_equal(opt)) {
                                        //arq_imm_next(opt);
                                        // for a cstr optional argument is not possible with bundling 
                                        // {'c', "cstring", fn_cstring, &ctx, "cstr_t = NULL"},
                                        // failure: -abcShello
                                        // ok:      -abcS hello
                                        char const *cstr 
                                        = (cmd->at[cmd->idx].id != ARQ_CMD_LONG_OPTION 
                                        && cmd->at[cmd->idx].id != ARQ_CMD_SHORT_OPTION) 
                                                ? cmd->at[cmd->idx].at 
                                                : NULL;
                                        arq_imm_next(opt);
                                        if (cstr != NULL) {
                                                next_bundle_idx(cmd);
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                } else {
                                        // for a cstr argument is possible with bundling 
                                        // {'c', "cstring", fn_cstring, &ctx, "cstr_t"},
                                        // 'hello' is the argument
                                        // ok: -abcShello
                                        // ok: -abcS hello
                                        if (cmd->at[cmd->idx].id == ARQ_END) {
                                                arq_msg_append_cstr(&error_msg, "CMD line failure:\n");
                                                arq_msg_append_cstr(&error_msg, "Token '");
                                                arq_msg_append_str(&error_msg, cmd->at[cmd->idx].at, cmd->at[cmd->idx].size);
                                                arq_msg_append_cstr(&error_msg, "' is not a c string");
                                                arq_msg_append_lf(&error_msg);
                                                end(&error_msg, &options[option_list->row]);
                                        } 

                                        // it looks like a short or long option but it is not it expects an argument
                                        char c = cmd->at[cmd->idx].at[-1];
                                        if (cmd->at[cmd->idx].id == ARQ_CMD_SHORT_OPTION) {
                                                if (c == '-') {
                                                        cmd->at[cmd->idx].at -= 1;    // -foo
                                                        cmd->at[cmd->idx].size += 1;
                                                }
                                        } else if (cmd->at[cmd->idx].id == ARQ_CMD_LONG_OPTION) {
                                             printf("helli\n");
                                             cmd->at[cmd->idx].at -= 2;   // --foo
                                             cmd->at[cmd->idx].size += 2;
                                        }
                                        char const *cstr = cmd->at[cmd->idx].at;
                                        next_bundle_idx(cmd);
                                        arq_push_cstr_t(queue, cstr);
                                }
                                continue;
                        }
#endif

                        if (opt->at[opt->idx].id == ARQ_PARA_COMMA) {
                                printf("ARQ_PARA_COMMA\n");
                                arq_imm_next(opt);
                                continue;
                        }

                        if (opt->at[opt->idx].id == ARQ_END) {
                                printf("ARQ_END\n\n");
                                call_option_fn(&options[option_list->row], queue);
                                opt = NULL;
                                if (cmd->at[cmd->idx].id == ARQ_END) {
                                        return;
                                } else {
                                        break;
                                }
                        }
                } // while 
        } // while
}

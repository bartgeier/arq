#include "arq.h"
#include "arq_symbols.h"
#include "arq_options.h"
#include "arq_queue.h"
#include "arq_cmd.h"
#include "arq_token.h"
#include "arq_conversion.h"
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

char stack_buffer[1000];
Arq_List_Vector *option_list = NULL;


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

static bool char_eq(Arq_Token *t, char a) {
        return a == (char)t->at[0];
}

static void call_option_fn(Arq_Option const * option, Arq_Queue *queue) {
        option->fn(option->context, queue);
}

static uint32_t next_idx(Arq_Vector *v, uint32_t idx) {
        if (idx < v->num_of_token - 1) {
                idx++;
        }
        return idx;
}

static uint32_t next_bundle_idx(Arq_Vector *v, uint32_t idx) {
        char const *const begin = v->at[idx].at;
        char const *const end = begin + strlen(begin);
        assert(*end == 0);
        char const *x = begin;
        while ((idx < v->num_of_token - 1) && (x >= begin) && (x < end)) {
                idx++;
                x = v->at[idx].at;
        }
        return idx;
}

void arq_fn(int argc, char **argv, Arq_Arena *arena, Arq_Option const *options, uint32_t const num_of_options) {
        option_list = (Arq_List_Vector *)arq_arena_malloc(arena, offsetof(Arq_List_Vector, at) + num_of_options * sizeof(Arq_Vector *));


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

                assert(option_list->num_of_tokenVec < num_of_options);
                option_list->at[option_list->num_of_tokenVec++] = v;

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

        uint32_t row = 0;
        bool found = false;
        hier gehts weiter mit der immediate Funktionalität
        uint32_t j = 0; // arq_imm_reset_vec(cmd) Arq_Vector needs read_idx
        uint32_t i = 0; // arq_imm_reset_list(options) Arq_List_Vector needs row_idx 
        while(j < cmd->num_of_token) {
                if (!found && cmd->at[j].id == ARQ_CMD_LONG_OPTION) {                         // arq_imm_is_long_option(cmd); arq_imm_is_short_option(cmd); arq_imm_is_end(cmd);
                        printf("ARQ_CMD_LONG_OPTION\n");                                      // Arq_Vector *opt = arq_imm_found(cmd, options);
                        for (uint32_t o = 0; o < num_of_options; o++) {                       // arq_imm_next(cmd); arq_imm_next(opt);
                                if (string_eq(&cmd->at[j], options[o].name)) {
                                        found = true;
                                        row = o;
                                        i = 0;
                                        break;
                                }
                        }
                        if (!found) { 
                                Error_msg_cmd_failure(&error_msg, "'--", &cmd->at[j], "' unknown long option ");
                                end(&error_msg, NULL);

                        }
                        j = next_idx(cmd, j);
                } else if (!found && cmd->at[j].id == ARQ_CMD_SHORT_OPTION) {
                        printf("ARQ_CMD_SHORT_OPTION\n");
                        for (uint32_t o = 0; o < num_of_options; o++) {
                                if (char_eq(&cmd->at[j], options[o].chr)) {
                                        found = true;
                                        row = o;
                                        i = 0;
                                        break;
                                }
                        }
                        if (!found) { 
                                Error_msg_cmd_failure(&error_msg, "'-", &cmd->at[j], "' unknown short option");
                                end(&error_msg, NULL);
                        }
                        j = next_idx(cmd, j);
                } else if (!found && cmd->at[j].id == ARQ_END) {
                        printf("cmd end!\n");
                        return;
                } else if (!found) {
                        Error_msg_cmd_failure(&error_msg, "'", &cmd->at[j], "' is not an option");
                        end(&error_msg, NULL);
                }
                Arq_Vector *opt = option_list->at[row];
                while (true) {
                        if (opt->at[i].id == ARQ_PARA_UINT32_T) {
                                i = next_idx(opt, i);
                                printf("ARQ_PARA_UINT32_T\n");
                                uint32_to num;
                                if (opt->at[i].id == ARQ_PARA_EQ) {
                                        i = next_idx(opt, i);
                                        num = arq_tok_uint32_t_to_uint32_t(&opt->at[i]);
                                        i = next_idx(opt, i);
                                        if (cmd->at[j].id == ARQ_P_NUMBER) {
                                                num = arq_tok_pNumber_to_uint32_t(&cmd->at[j], &error_msg, "CMD line failure:\n");
                                                if (num.error) { end(&error_msg, &options[row]); }
                                                j = next_idx(cmd, j);
                                        }
                                } else {
                                        i = next_idx(opt, i);
                                        num = arq_tok_to_uint32_t(&cmd->at[j], &error_msg, "CMD line failure:\n");
                                        if (num.error) { end(&error_msg, &options[row]); }
                                        j = next_idx(cmd, j);
                                }
                                arq_push_uint32_t(queue, num.u32);
                                printf("u32 %d\n", num.u32);
                                continue;
                        }

                        if (opt->at[i].id == ARQ_PARA_CSTR_T) {
                                i = next_idx(opt, i);
                                printf("ARQ_PARA_CSTR_T\n");
                                if (opt->at[i].id == ARQ_PARA_EQ) {
                                        i = next_idx(opt, i);
                                        char const *cstr = (cmd->at[j].id != ARQ_CMD_LONG_OPTION && cmd->at[j].id != ARQ_CMD_SHORT_OPTION) ? cmd->at[j].at : NULL;
                                        i = next_idx(opt, i);
                                        if (cstr != NULL) {
                                                j = next_bundle_idx(cmd, j);
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                } else {
                                        i = next_idx(opt, i);
                                        if (cmd->at[j].id == ARQ_END) {
                                                arq_msg_append_cstr(&error_msg, "CMD line failure:\n");
                                                arq_msg_append_cstr(&error_msg, "Token '");
                                                arq_msg_append_str(&error_msg, cmd->at[j].at, cmd->at[j].size);
                                                arq_msg_append_cstr(&error_msg, "' is not a c string");
                                                arq_msg_append_lf(&error_msg);
                                                end(&error_msg, &options[row]);
                                        } 
                                        if (cmd->at[j].id == ARQ_CMD_SHORT_OPTION) {
                                             cmd->at[j].at -= 1;    // -foo
                                             cmd->at[j].size += 1;
                                        } else if (cmd->at[j].id == ARQ_CMD_LONG_OPTION) {
                                             cmd->at[j].at -= 2;   // --foo
                                             cmd->at[j].size += 2;
                                        }
                                        char const *cstr = cmd->at[j].at;
                                        j = next_bundle_idx(cmd, j);
                                        arq_push_cstr_t(queue, cstr);
                                }
                                continue;
                        }

                        if (opt->at[i].id == ARQ_PARA_COMMA) {
                                printf("ARQ_PARA_COMMA\n");
                                i = next_idx(opt, i);
                                continue;
                        }

                        if (opt->at[i].id == ARQ_END) {
                                printf("ARQ_END\n\n");
                                call_option_fn(&options[row], queue);
                                found = false;
                                if (cmd->at[j].id == ARQ_END) {
                                        return;
                                } else {
                                        break;
                                }
                        }
                } // while 
        } // while
} 

// todo memory management

#if 0 
typedef struct {
        const char *name;        // --version
        char chr;                // -v
        function_pointer_t fn;
        void* self;              // context
        const char *arguments;   // "uint8_t, bool = false"
} Arq_Option;
#endif


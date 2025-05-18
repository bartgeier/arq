//#include "arq.h"
#include "arq_symbols.h"
#include "arq_options.h"
#include "arq_stack.h"
#include "arq_cmd.h"
#include "arq_token.h"
#include "arq_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

char stack_buffer[1000];

Arq_List_Vector *option_list = NULL;

static void print_token(Arq_Token *t) {
        printf("%3d %30s -> ", t->size, symbol_names[t->id]);
        for (uint32_t i = 0; i < t->size; i++) {
                putchar(t->at[i]);
        }
        printf("\n");
}

static bool char_eq(Arq_Token *t, char a) {
        return a == (char)t->at[0];
}

void call_option_fn(Arq_Option const * option) {
        option->fn(option->self);
}

static uint32_t next_idx(Arq_Vector *v, uint32_t idx) {
        if (idx < v->num_of_token - 1) {
                idx++;
        }
        return idx;
}

void arq_fn(int argc, char **argv, Arq_Option const *options, uint32_t const num_of_options) {
        option_list = (Arq_List_Vector *)malloc(sizeof(Arq_List_Vector) + num_of_options * sizeof(Arq_Vector));

        uint32_t n = arq_stack_init(&stack_buffer, sizeof(stack_buffer));
        printf("Max possible arguments %d\n",n);

        for (uint32_t i = 0; i < num_of_options; i++) {
                uint32_t num_of_token = arq_num_of_option_token(&options[i]);
                Arq_Vector *v = (Arq_Vector *)calloc(
                        1,
                        sizeof(Arq_Vector) + num_of_token * sizeof(Arq_Token)
                );

                arq_tokenize_option(&options[i], v, num_of_token);

                assert(option_list->num_of_tokenVec < num_of_options);
                option_list->at[option_list->num_of_tokenVec++] = v;

                printf("Option %d:\n", i);
                for (uint32_t j = 0; j < v->num_of_token; j++) {
                        print_token(&v->at[j]);
                }
        }
        printf("\n");

///////////////////////////////////////////////////////////////////////////////

        uint32_t num_of_token = arq_num_of_cmd_token(argc, argv);
        Arq_Vector *cmd = (Arq_Vector *)calloc(
                1,
                sizeof(Arq_Vector) + num_of_token * sizeof(Arq_Token)
        );
        arq_tokenize_cmd(argc, argv, cmd, num_of_token);

        printf("Command line: %d \n", num_of_token);
        for (uint32_t i = 0; i < cmd->num_of_token; i++) {
                print_token(&cmd->at[i]);
        }
        printf("\n");
        
///////////////////////////////////////////////////////////////////////////////

        uint32_t row = 0;
        bool found = false;
        uint32_t j = 0;
        uint32_t i = 0;
        while(j < cmd->num_of_token) {
                if (!found && cmd->at[j].id == ARQ_CMD_LONG_OPTION) {
                        printf("ARQ_CMD_LONG_OPTION ");
                        for (uint32_t o = 0; o < num_of_options; o++) {
                                if (string_eq(&cmd->at[j], options[o].name)) {
                                        printf("found\n");
                                        found = true;
                                        row = o;
                                        i = 0;
                                        break;
                                }
                        }
                        assert(found && "unknown long option"); // todo special assert
                        j = next_idx(cmd, j);
                } else if (!found && cmd->at[j].id == ARQ_CMD_SHORT_OPTION) {
                        printf("ARQ_CMD_SHORT_OPTION");
                        for (uint32_t o = 0; o < num_of_options; o++) {
                                if (char_eq(&cmd->at[j], options[o].chr)) {
                                        printf("found\n");
                                        found = true;
                                        row = o;
                                        i = 0;
                                        break;
                                }
                        }
                        assert(found && "unknown short option"); // todo special assert
                        j = next_idx(cmd, j);
                } else if (!found && cmd->at[j].id == ARQ_END) {
                        printf("cmd end!\n");
                        return;
                } else if (!found) {
                        j = next_idx(cmd, j);
                        printf("no option?\n\n");
                        continue;
                }
                Arq_Vector *opt = option_list->at[row];
                while (true) {
                        if (opt->at[i].id == ARQ_PARA_UINT32_T) {
                                i = next_idx(opt, i);
                                printf("ARQ_PARA_UINT32_T\n");
                                uint32_t u32 = 0;
                                if (opt->at[i].id == ARQ_PARA_EQ) {
                                        i = next_idx(opt, i);
                                        u32 = string_to_uint32(&opt->at[i++]);
                                        if (cmd->at[j].id == ARQ_P_NUMBER) {
                                                bool overflow;
                                                u32 = string_to_uint32_safe(&cmd->at[j], &overflow); // get uint32_t num argument from cmd
                                                if (overflow) {
                                                        printf("Number overflows uint32_t\n");
                                                }
                                                j = next_idx(cmd, j);
                                        }
                                        arq_push_uint32_t(u32);
                                        printf("u32 %d\n", u32);
                                        continue;
                                } else {
                                        if (cmd->at[j].id != ARQ_P_NUMBER) {
                                                printf("cmd->at[j].id != ARQ_P_NUMBER\n");
                                                exit(1);
                                        }
                                        bool overflow;
                                        u32 = string_to_uint32_safe(&cmd->at[j], &overflow); // get uint32_t num argument from cmd
                                        j = next_idx(cmd, j);
                                        if (overflow) {
                                                printf("Number overflows uint32_t\n");
                                        }
                                        arq_push_uint32_t(u32);
                                        printf("u32 %d\n", u32);
                                        continue;
                                }
                        }

                        if (opt->at[i].id == ARQ_PARA_COMMA) {
                                printf("ARQ_PARA_COMMA\n");
                                i = next_idx(opt, i);
                                continue;
                        }

                        if (opt->at[i].id == ARQ_END) {
                                //call_option_fn(&options[row]); 
                                printf("ARQ_END\n\n");
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

#if 0 
typedef struct {
        const char *name;        // --version
        char chr;                // -v
        function_pointer_t fn;
        void* self;              // context
        const char *arguments;   // "uint8_t, bool = false"
} Arq_Option;
#endif


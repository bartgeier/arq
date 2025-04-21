//#include "arq.h"
#include "arq_symbols.h"
#include "arq_options.h"
#include "arq_stack.h"
#include "arq_cmd.h"
#include "arq_token.h"
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

static bool string_eq(char const *a, Arq_Token *t) {
        if (strlen(a) != t->size) {
                return false;
        }
        for (uint32_t i = 0; i < t->size; i++) {
                if (a[i] != t->at[i]) {
                        return false;
                }
        }
        return true;
}

static bool char_eq(char a, Arq_Token *t) {
        return a == (char)t->at[0];
}

void call_option_fn(Arq_Option const * option) {
        option->fn(option->self);
}

void arq_fn(int argc, char **argv, Arq_Option *options, uint32_t num_of_options) {
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

                printf("\nOption %d:\n", i);
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

        printf("Command line:\n");
        for (uint32_t i = 0; i < cmd->num_of_token; i++) {
                print_token(&cmd->at[i]);
        }
        printf("\n");

///////////////////////////////////////////////////////////////////////////////

        struct {
                uint32_t n;
                uint32_t v;
                bool ok;
        } idx = {0};

        for (uint32_t i = 0; i < cmd->num_of_token; i++) {

#if 1 
                if (idx.ok) {
                        Arq_Vector *v = option_list->at[idx.n];
                        Arq_Token expected = v->at[idx.v++];
                        if (expected.id == ARQ_PARA_UINT32_T) {
                                uint32_t u32;
                                if (is_para_eq(v, idx.v++)
                                && is_para_p_number(v, idx.v)) {
                                        u32 = get_para_u32(v, idx.v++);
                                        if (is_cmd_u32(cmd, i) {
                                                u32 = get_cmd_u32(cmd, i);
                                        }
                                }

                        }
                }
#endif
                if (cmd->at[i].id == ARQ_CMD_LONG_OPTION) {
                        for (uint32_t j = 0; j < num_of_options; j++) {
                                if (string_eq(options[j].name, &cmd->at[i])) {
                                        if (idx.ok) { 
                                                call_option_fn(&options[idx.n]); 
                                        }
                                        idx.ok = true;
                                        idx.n = j;
                                }
                        }
                }
                if (cmd->at[i].id == ARQ_CMD_SHORT_OPTION) {
                        for (uint32_t j = 0; j < num_of_options; j++) {
                                if (char_eq(options[j].chr, &cmd->at[i])) {
                                        if (idx.ok) { 
                                                call_option_fn(&options[idx.n]); 
                                        }
                                        idx.ok = true;
                                        idx.n = j;
                                }
                        }
                }
                //printf("First token is not an Option\n");
        }
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

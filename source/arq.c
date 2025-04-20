//#include "arq.h"
#include "arq_symbols.h"
#include "arq_options.h"
#include "arq_stack.h"
#include "arq_cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char stack_buffer[1000];

Arq_List_Vector *option_list = NULL;

static void print_token(Arq_Token *t) {
        printf("%3d %30s -> ", t->size, symbol_names[t->id]);
        for (uint32_t i = 0; i < t->size; i++) {
                putchar(t->at[i]);
        }
        printf("\n");
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

        Arq_Token b0[20];
        Arq_VectorBuilder builder = {
                .NUM_OF_TOKEN = sizeof(b0)/sizeof(Arq_Token),
                .num_of_token = 0,
                .at = b0,
        };

        arq_tokenize_cmd(argc, argv, &builder);

        printf("Command line:\n");
        for (uint32_t i = 0; i < builder.num_of_token; i++) {
                print_token(&builder.at[i]);
        }
        printf("\n");

///////////////////////////////////////////////////////////////////////////////

}

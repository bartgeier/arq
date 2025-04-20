//#include "arq.h"
#include "arq_symbols.h"
#include "arq_options.h"
#include "arq_stack.h"
#include "arq_cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char stack_buffer[1000];

List_of_OTokenVec *option_list = NULL;

static void print_token(OToken *t) {
        printf("%3d %30s -> ", t->size, symbol_names[t->id]);
        for (uint32_t i = 0; i < t->size; i++) {
                putchar(t->at[i]);
        }
        printf("\n");
}

void arq_fn(int argc, char **argv, Arq_Option *options, uint32_t num_of_options) {
        option_list = (List_of_OTokenVec *)malloc(sizeof(List_of_OTokenVec) + num_of_options * sizeof(OTokenVec));

        uint32_t n = arq_stack_init(&stack_buffer, sizeof(stack_buffer));
        printf("Max possible arguments %d\n",n);

        OToken b0[20];
        OTokenBuilder tb = {
                .NUM_OF_TOKEN = sizeof(b0)/sizeof(OToken),
                .num_of_token = 0,
                .at = b0,
        };

        for (uint32_t i = 0; i < num_of_options; i++) {
                tb.num_of_token = 0;
                arq_compile_option(&options[i], &tb);
                OTokenVec *tv = (OTokenVec *)malloc(sizeof(OTokenVec) + tb.num_of_token * sizeof(OToken));
                tv->num_of_token = tb.num_of_token;
                memcpy(tv->at, tb.at, tb.num_of_token * sizeof(OToken));
                option_list->at[option_list->num_of_tokenVec++] = tv;

                printf("\nOption %d:\n", i);
                for (uint32_t j = 0; j < tv->num_of_token; j++) {
                        print_token(&tv->at[j]);
                }
        }
        printf("\n");

///////////////////////////////////////////////////////////////////////////////

        tb.num_of_token = 0;
        arq_compile_cmd(argc, argv, &tb);

        printf("Command line:\n");
        for (uint32_t i = 0; i < tb.num_of_token; i++) {
                print_token(&tb.at[i]);
        }
        printf("\n");

///////////////////////////////////////////////////////////////////////////////

}

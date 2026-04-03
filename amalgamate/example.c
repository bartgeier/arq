/* #define ARQ_LOG_TOKENS    */
/* #define ARQ_LOG_TOKENIZER */
/* #define ARQ_LOG_MEMORY    */
#define ARQ_IMPLEMENTATION
#include "arq.h"
#include <stdio.h>
#include <memory.h>
#include <assert.h>

void fn_cstring(Arq_Queue *queue) {
        char const *str = arq_cstr_t(queue);
        printf("-c --cstring\n");
        assert(str != NULL);
        printf("str = %s\n", str);
}

void fn_optionalstr(Arq_Queue *queue) {
        char const *str = arq_cstr_t(queue);
        printf("-o --optionalstr\n");
        if (str == NULL) {
                printf("str = NULL\n");
        } else {
                printf("str = %s\n", str);
        }
}

void fn_uint(Arq_Queue *queue) {
        uint32_t number = arq_uint(queue);
        printf("-u --uint\n");
        printf("number = %u\n", number);
}

void fn_uintdefault(Arq_Queue *queue) {
        uint32_t number = arq_uint(queue);
        printf("-U --uint-default\n");
        printf("number = %u\n", number);
}

void fn_int(Arq_Queue *queue) {
        int32_t number = arq_int(queue);
        printf("-i --int\n");
        printf("number = %d\n", number);
}

void fn_intdefault(Arq_Queue *queue) {
        int32_t number = arq_int(queue);
        printf("-I --int_default\n");
        printf("number = %d\n", number);
}

void fn_intarray(Arq_Queue *queue) {
        printf("-j --int_array\n");
        {
                uint32_t i;
                uint32_t const array_size = arq_array_size(queue);
                printf("\n");
                printf("list array_size: %d\n", array_size);
                for (i = 0; i < array_size; i++) {
                        printf("    argument[%d]: %d\n", i, arq_int(queue));
                }
        }
}

void fn_float(Arq_Queue *queue) {
        double number = arq_float(queue);
        printf("-f --float\n");
        printf("fn_float number = %.10f\n", number);
}

void fn_floatdefault(Arq_Queue *queue) {
        double number = arq_float(queue);
        printf("-F --floatdefault\n");
        printf("number = %.10f\n", number);
}

void fn_multiple(Arq_Queue *queue) {
        uint32_t begin = arq_uint(queue);
        uint32_t end = arq_uint(queue);
        printf("-m --multiple\n");
        printf("begin = %d\nend = %d\n", begin, end);
}

void fn_mixed(Arq_Queue *queue) {
        uint32_t u_nr = arq_uint(queue);
        int32_t i_nr = arq_int(queue);
        double f_nr = arq_float(queue);
        char const *comment = arq_cstr_t(queue);
        printf("-x --mixed\n");
        printf("u_nr = %u\ni_nr = %d\nf_nr = %f\ncomment = %s\n", u_nr, i_nr, f_nr, comment);
}

void fn_positionalargument(Arq_Queue *queue) {
        printf("-p --positional_argument\n");
        {
                int32_t const number = arq_int(queue);
                printf("numbere: %d\n", number);
        } {
                uint32_t i;
                uint32_t const array_size = arq_array_size(queue);
                printf("\n");
                printf("list array_size: %d\n", array_size);
                for (i = 0; i < array_size; i++) {
                        char const *list = arq_cstr_t(queue);
                        printf("    list[%d]: %s\n", i, list);
                }
        }
}

void fn_help(Arq_Queue *queue);

Arq_Option options[] = {
        {'h', "help",         fn_help, "()"},

        {'c', "cstring",      fn_cstring, "(cstr_t str)"},
        {'o', "optionalstr",  fn_optionalstr, "(cstr_t str = NULL)"},

        {'u', "uint",         fn_uint, "(uint number)"},
        {'U', "uint-default", fn_uintdefault, "(uint number = 69)"}, 

        {'i', "int",          fn_int, "(int number)"}, 
        {'I', "int_default",  fn_intdefault, "(int number = -69)"}, 
        {'j', "int_array",    fn_intarray, "(int numbers[])"},

        {'f', "float",        fn_float, "(float number)"}, 
        {'F', "floatdefault", fn_floatdefault, "(float number = 5.1e1)"}, 

        {'m', "multiple",     fn_multiple, "(uint first_line = 0, uint last_line = +1200)"},
        {'x', "mixed",        fn_mixed, "(uint u_nr, int i_nr, float f_nr, cstr_t comment)"},

        {'p', "positional_argument", fn_positionalargument, "(int number, cstr_t list[])"},
};

void fn_help(Arq_Queue *queue) {
        size_t i;
        (void) queue;
        printf("help\n");
        for (i = 0; i < sizeof(options)/sizeof(Arq_Option); i++) {
                printf("-%c --%s%s\n", options[i].chr, options[i].name, options[i].arguments);
        }
}

int main(int argc, char **argv) {
        char buffer[10000];
        if (0 < arq_verify(buffer, sizeof(buffer), options, sizeof(options)/sizeof(Arq_Option))) {
                /* arq_verify test Arq_Option options[] for errors */
                /* arq_verify returns strlen of error msg */
                /* print error msg */
                printf("%s\n", (char *)buffer);
                return 1;
        }
        if (0 < arq_fn(argc, argv, buffer, sizeof(buffer), options, sizeof(options)/sizeof(Arq_Option))) {
                /* arq_fn calls calback function of Arq_Option options[] */
                /* arq_fn returns strlen of error msg */
                /* print error msg */
                printf("%s\n", (char *)buffer);
                return 1;
        }
        printf("\nbyby\n");
        return 0;
}

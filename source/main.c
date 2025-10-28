#include <stdio.h>
#include "arq_bool.h"
#include "arq_int.h"

#include "arq.h"
#include <memory.h>
#include <assert.h>

typedef struct {
        bool help;
        uint8_t number;
} Context;

void fn_version(Arq_Queue *queue) {
        (void) queue;
        printf("version 1.1.1 \n");
}

void fn_string(Arq_Queue *queue) {
        char const *str = arq_cstr_t(queue);
        assert(str != NULL);
        printf("fn_string str = %s\n", str);
}

void fn_nstring(Arq_Queue *queue) {
        char const *str = arq_cstr_t(queue);
        if (str != NULL) {
                printf("fn_nstring s = %s\n", str);
        } else {
                printf("fn_nstring s = NULL\n");
        }
}

void fn_uint32(Arq_Queue *queue) {
        uint32_t number = arq_uint32_t(queue);
        printf("fn_uint32 number = %d\n", number);
}

void fn_int32(Arq_Queue *queue) {
        int32_t number = arq_int32_t(queue);
        printf("fn_int32 number = %d\n", number);
}

void fn_print(Arq_Queue *queue) {
        uint32_t begin = arq_uint32_t(queue);
        uint32_t end = arq_uint32_t(queue);
        printf("fn_print begin = %d, end = %d\n", begin, end);
}

void fn_array(Arq_Queue *queue) {
        printf("fn_array {\n");
        {
                uint32_t const array_size = arq_array_size(queue);
                uint32_t i;
                printf("    numbers array_size: %d\n", array_size);
                for (i = 0; i < array_size; i++) {
                        printf("        argument[%d]: %d\n", i, arq_int32_t(queue));
                }
        } {
                uint32_t const array_size = arq_array_size(queue);
                uint32_t i;
                printf("\n");
                printf("    foo array_size: %d\n", array_size);
                for (i = 0; i < array_size; i++) {
                        printf("        argument[%d]: %s\n", i, arq_cstr_t(queue));
                }
        }
        printf("}\n");
}

void fn_test(Arq_Queue *queue) {
        uint32_t num_0 = arq_uint32_t(queue);
        uint32_t num_1 = arq_uint32_t(queue);
        printf("fn_test %d %d\n", num_0, num_1);
}


int main(int argc, char **argv) {
        Arq_Option options[] = {
                {'v', "version", fn_version, "()"},
                {'s', "string",  fn_string,  "(cstr_t str)"},
                {'n', "nstring", fn_nstring, "(cstr_t str = NULL)"},
                {'u', "uint32",  fn_uint32,  "(uint32_t number)"},
                {'i', "int32",   fn_int32,   "(int32_t number = 0xFFFFFFFF)"}, 

                {'p', "print",   fn_print,   "(uint32_t first_line = 0, uint32_t last_line = +1200)"},
                {'a', "array",   fn_array,   "(int32_t numbers[], cstr_t list[])"},
        };

        /* testen mit */
        /* ./nob && build/arq -v -t 4 5 --sstring  f --cstring hello */
        char buffer[10000];

        printf("size of size_t %ld\n", sizeof(size_t));
        if (0 < arq_fn(
                argc, argv, 
                buffer, sizeof(buffer),
                options, sizeof(options)/sizeof(Arq_Option))
        ) {
                /* arq_fn returns strlen of error msg */
                /* print error msg */
                printf("%s\n", (char *)buffer);
        }

        return 0;
}

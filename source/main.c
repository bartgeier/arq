#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "arq.h"
#include <memory.h>

typedef struct {
        bool help;
        uint8_t number;
} Context;

void fn_print(Arq_Queue *queue) {
        uint32_t begin = arq_uint32_t(queue);
        uint32_t end = arq_uint32_t(queue);
        printf("fn_print begin = %d, end = %d\n", begin, end);
}

void fn_version(Arq_Queue *queue) {
        (void) queue;
        printf("version 1.1.1 \n");
}

void fn_array(Arq_Queue *queue) {
        (void) queue;
        printf("array \n");
}

void fn_test(Arq_Queue *queue) {
        uint32_t num_0 = arq_uint32_t(queue);
        uint32_t num_1 = arq_uint32_t(queue);
        printf("fn_test %d %d\n", num_0, num_1);
}

void fn_cstring(Arq_Queue *queue) {
        char const *cstr_a = arq_cstr_t(queue);
        if (cstr_a != NULL) {
                printf("fn_cstring cstr_a: %s\n", cstr_a);
        } else {
                printf("fn_cstring\n");
        }
}

void fn_sstring(Arq_Queue *queue) {
        char const *cstr_a = arq_cstr_t(queue);
        //char const *cstr_b = arq_cstr_t(queue);
        //printf("fn_sstring cstr_a: %s, cstr_b: %s\n", cstr_a, cstr_b);
        if (cstr_a != NULL) {
                printf("fn_sstring cstr_a: %s\n", cstr_a);
        } else {
                printf("fn_cstring\n");
        }
}

int main(int argc, char **argv) {
        Arq_Option options[] = {
                {'v', "version", fn_version, "()"},
                {'a', "array",   fn_array,   "(uint32_t numbers)"},
                {'p', "print",   fn_print,   "(uint32_t first_line = 0, uint32_t last_line = 1200)"},
                {'t', "test",    fn_test,    "(uint32_t number, uint32_t offset)"},
                {'c', "cstring", fn_cstring, "(cstr_t cstring = NULL)"},
                {'s', "sstring", fn_sstring, "(cstr_t sstring)"},
        };

        // testen mit
        // ./nob && build/arq -v -t 4 5 --sstring  f --cstring hello 
       char buffer[1000];

        if (0 < arq_fn(
                argc, argv, 
                buffer, sizeof(buffer),
                options, sizeof(options)/sizeof(Arq_Option))
        ) {
                // arq_fn returns strlen of error msg
                // print error msg
                printf("%s\n", (char *)buffer);
        }

        return 0;
}

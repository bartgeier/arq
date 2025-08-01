#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "arq.h"
#include "arq_arena.h"
#include <memory.h>

typedef struct {
        bool help;
        uint8_t number;
} Context;

void fn_print(void *context, Arq_Queue *queue) {
        (void)context;
        uint32_t begin = arq_uint32_t(queue);
        uint32_t end = arq_uint32_t(queue);
        printf("fn_print begin = %d, end = %d\n", begin, end);
}

void fn_version(void *context, Arq_Queue *queue) {
        (void)context;
        (void) queue;
        printf("version 1.1.1 \n");
}

void fn_test(void *context, Arq_Queue *queue) {
        (void)context;
        uint32_t num_0 = arq_uint32_t(queue);
        uint32_t num_1 = arq_uint32_t(queue);
        printf("fn_test %d %d\n", num_0, num_1);
}

void fn_cstring(void *context, Arq_Queue *queue) {
        (void)context;
        char const *cstr_a = arq_cstr_t(queue);
        printf("fn_cstring cstr_a: %s\n", cstr_a);
        //printf("fn_cstring\n");
}

void fn_sstring(void *context, Arq_Queue *queue) {
        (void)context;
        char const *cstr_a = arq_cstr_t(queue);
        char const *cstr_b = arq_cstr_t(queue);
        printf("fn_sstring cstr_a: %s, cstr_b: %s\n", cstr_a, cstr_b);
        //printf("fn_cstring\n");
}

int main(int argc, char **argv) {
        // printf("%s\n", argv[1]);
        // return 0;
        // (void) argc;
        // (void) argv;
        Context ctx = { .help = false, };

        Arq_Option options[] = {
                {'v', "version", fn_version, &ctx, ""},
                {'p', "print", fn_print, &ctx, "uint32_t=3, uint32_t = 4"},
                {'t', "test",  fn_test, &ctx, "uint32_t, uint32_t"},
                {'c', "cstring", fn_cstring, &ctx, "cstr_t = NULL"},
                {'s', "sstring", fn_sstring, &ctx, "cstr_t, cstr_t = NULL"},
        };
        char buffer[10000];
        memset(buffer, 0xFF, sizeof(buffer));
        Arq_Arena *arena = arq_arena_init(&buffer, sizeof(buffer));
        arq_fn(argc, argv, arena, options, sizeof(options)/sizeof(Arq_Option));
        return 0;
}

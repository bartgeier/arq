#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "arq.h"
#include "arq_arena.h"
#include "arq_stack.h"
//#include "arq_options.h"
//#include "arq_cmd.h"

typedef struct {
        bool help;
        uint8_t number;
} Context;

void fn_print(void *self) {
        (void)self;
        uint32_t begin = arq_uint32_t();
        uint32_t end = arq_uint32_t();
        printf("fn_print begin = %d, end = %d\n", begin, end);
}

void fn_version(void *self) {
        (void)self;
        printf("version 1.1.1 \n");
}

void fn_test(void *self) {
        (void)self;
        uint32_t num_0 = arq_uint32_t();
        uint32_t num_1 = arq_uint32_t();
        printf("fn_test %d %d\n", num_0, num_1);
}

void fn_cstring(void *self) {
        (void)self;
        char const *cstr_a = arq_cstr_t();
        char const *cstr_b = arq_cstr_t();
        printf("fn_cstring cstr_a: %s, cstr_b: %s\n", cstr_a, cstr_b);
        //printf("fn_cstring\n");
}

int main(int argc, char **argv) {
        // printf("%s\n", argv[1]);
        // return 0;
        (void) argc;
        (void) argv;
        Context self = { .help = false, };

        Arq_Option options[] = {
                {'v', "version", fn_version, &self, ""},
                {'p', "print", fn_print, &self, "uint32_t=3, uint32_t = 4"},
                {'t', "test",  fn_test, &self, "uint32_t, uint32_t"},
                {'c', "cstring", fn_cstring, &self, "cstr_t , cstr_t = NULL"},
        };
        char *buffer[10000] = {0};
        ArqArena *arena = arq_arena_init(&buffer, sizeof(buffer));
        arq_fn(argc, argv, arena, options, sizeof(options)/sizeof(Arq_Option));

       // arq_compile_options(options, 3);

       // arq_compile_cmd(argc, argv);

        // options[0].fn(options[0].self);
        // options[1].fn(options[1].self);

        // arq_push_uint32_t(68);
        // options[2].fn(options[2].self);

        
        return 0;
}

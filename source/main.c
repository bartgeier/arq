#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "arq.h"
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


int main(int argc, char **argv) {
        // printf("%s\n", argv[1]);
        // return 0;
        (void) argc;
        (void) argv;
        Context self = { .help = false, };

        //char buffer[1000];
        Arq_Option options[] = {
                {'p', "print", fn_print, &self, "uint32_t = 3, uint32_t = 43"},
                {'t', "test",  fn_test, &self, "uint32_t, uint32_t"},
                // {'v', "version", fn_version, &self, "uint32_t"},
                // {'p', "print", fn_print, &self, "uint32_t, uint32_t = 1000"},
        };
        arq_fn(argc, argv, options, sizeof(options)/sizeof(Arq_Option));

       // arq_compile_options(options, 3);

       // arq_compile_cmd(argc, argv);

        // options[0].fn(options[0].self);
        // options[1].fn(options[1].self);

        // arq_push_uint32_t(68);
        // options[2].fn(options[2].self);

        
        return 0;
}

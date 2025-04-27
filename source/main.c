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

void fn_help(void *self) {
        Context *m = (Context *)self;
        m->help = true;
        printf("help %d\n", m->help);
}

void fn_version(void *self) {
        (void)self;
        printf("version 1.1.1 \n");
}

void fn_test(void *self) {
        Context *m = (Context *)self;
        m->number = arq_uint32_t();
        printf("A number %d\n", m->number);
}


int main(int argc, char **argv) {
        // printf("%s\n", argv[1]);
        // return 0;
        (void) argc;
        (void) argv;
        Context self = { .help = false, };

        //char buffer[1000];
        Arq_Option options[] = {
                {"help", 'h', fn_help, &self, "uint32_t=5, uint32_t = 11"},
                {"version", 'v', fn_version, &self, "bool"},
                {"test", 't', fn_test, &self, "345"},
                // {"print", 'p', fn_print, &self, "uint32_t, uint32_t = 1000"},
        };
        arq_fn(argc, argv, options, sizeof(options)/sizeof(Arq_Option));

       // arq_compile_options(options, 3);

       // arq_compile_cmd(argc, argv);

        options[0].fn(options[0].self);
        options[1].fn(options[1].self);

        arq_push_uint32_t(68);
        options[2].fn(options[2].self);

        
        return 0;
}

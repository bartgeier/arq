#ifndef ARQ_H
#define ARQ_H

#include <stdint.h>

typedef void (*function_pointer_t)(void *self);
typedef struct {
        char chr;                // -v
        const char *name;        // --version
        function_pointer_t fn;
        void* self;              // context
        const char *arguments;   // "uint8_t, bool = false"
} Arq_Option;

void arq_fn(int argc, char **argv, Arq_Option const *options, uint32_t const num_of_options);

void arq_unused(void);
uint8_t arq_uint8_t(void);
uint16_t arq_uint16_t(void);
uint32_t arq_uint32_t(void);
uint64_t arq_uint64_t(void);
int8_t arq_int8_t(void);
int16_t arq_int16_t(void);
int32_t arq_int32_t(void);
char const *arq_cstr_t(void);

#endif

#ifndef ARQ_H
#define ARQ_H

#include "arq_int.h"

typedef struct Arq_Queue_tag Arq_Queue;

typedef void (*function_pointer_t)(Arq_Queue *queue);
typedef struct {
        char chr;                /* -v        */
        const char *name;        /* --version */
        function_pointer_t fn;
        const char *arguments;   /* "uint8_t, bool = false" */
} Arq_Option;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t arq_fn(
        int argc, char **argv, 
        char *arena_buffer, uint32_t const buffer_size,
        Arq_Option const *options, uint32_t const num_of_options
);

void arq_unused(Arq_Queue *queue);
uint32_t arq_uint32_t(Arq_Queue *queue);
uint32_t arq_array_size(Arq_Queue *queue);
/* uint64_t arq_uint64_t(Arq_Queue *queue); */
int32_t arq_int32_t(Arq_Queue *queue);
double arq_float(Arq_Queue *queue);
char const *arq_cstr_t(Arq_Queue *queue);

#ifdef __cplusplus
}
#endif
#endif

#ifndef ARQ_H
#define ARQ_H

#include "arq_arena.h"
#include <stdint.h>

typedef struct Arq_Queue_tag Arq_Queue;

typedef void (*function_pointer_t)(void *context, Arq_Queue *queue);
typedef struct {
        char chr;                // -v
        const char *name;        // --version
        function_pointer_t fn;
        void* context;
        const char *arguments;   // "uint8_t, bool = false"
} Arq_Option;

#ifdef __cplusplus
extern "C" {
#endif

void arq_fn(int argc, char **argv, Arq_Arena *arena, Arq_Option const *options, uint32_t const num_of_options);
void arq_error_msg_callback(char const *error_msg);

void arq_unused(Arq_Queue *queue);
uint8_t arq_uint8_t(Arq_Queue *queue);
uint16_t arq_uint16_t(Arq_Queue *queue);
uint32_t arq_uint32_t(Arq_Queue *queue);
uint64_t arq_uint64_t(Arq_Queue *queue);
int8_t arq_int8_t(Arq_Queue *queue);
int16_t arq_int16_t(Arq_Queue *queue);
int32_t arq_int32_t(Arq_Queue *queue);
char const *arq_cstr_t(Arq_Queue *queue);

#ifdef __cplusplus
}
#endif
#endif

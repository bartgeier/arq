#ifndef ARQ_ARENA_H
#define ARQ_ARENA_H

#include <stdint.h>

#define ARQ_ARENA_SIZE_OF_PADDING sizeof(uint64_t)

typedef struct {
        uint32_t const SIZE;
        uint32_t size;
        uint32_t stack_size;
        char *stack;
        char at[1];
} ArqArena;

ArqArena *arq_arena_init(void *buffer, uint32_t const size);
void *arq_arena_malloc(ArqArena *m, uint32_t const num_of_bytes);
void *arq_arena_malloc_rest(ArqArena *m, uint32_t const size_of_header, uint32_t const size_of_element, uint32_t *num_of_elements);

#endif

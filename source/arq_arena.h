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

#endif

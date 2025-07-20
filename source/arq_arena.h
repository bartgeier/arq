#ifndef ARQ_ARENA_H
#define ARQ_ARENA_H

#include <stdint.h>

#define ARQ_ARENA_SIZE_OF_PADDING sizeof(uint64_t)

typedef struct {
        uint32_t const SIZE;
        uint32_t size;
        char at[1];
} Arq_Arena;

#ifdef __cplusplus
extern "C" {
#endif

Arq_Arena *arq_arena_init(void *buffer, uint32_t const size);
void *arq_arena_malloc(Arq_Arena *m, uint32_t const num_of_bytes);
void *arq_arena_malloc_rest(Arq_Arena *m, uint32_t const size_of_header, uint32_t const size_of_element, uint32_t *num_of_elements);

#ifdef __cplusplus
}
#endif
#endif

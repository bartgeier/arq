#include "arq_arena.h"

#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

ArqArena *arq_arena_init(void *buffer, uint32_t const _size) {
        uint32_t const offset = (uintptr_t)buffer % ARQ_ARENA_SIZE_OF_PADDING;
        uint32_t padding = offset > 0 ? ARQ_ARENA_SIZE_OF_PADDING - offset : 0;
        uint32_t const size = _size - padding;
        ArqArena *m = (ArqArena *)((char*)buffer + padding);
        assert((uintptr_t)m % ARQ_ARENA_SIZE_OF_PADDING == 0 && "buffer does not align");

        // uint32_t header_size = 
        // + sizeof(m->SIZE) 
        // + sizeof(m->size) 
        // + sizeof(m->stack_size)
        // + sizeof(m->stack); 
        uint32_t const header_size = offsetof(ArqArena, at);

        assert(size > header_size);
        uint32_t size_memory_that_is_usable = size - header_size;
        memcpy((void *)&m->SIZE, &size_memory_that_is_usable, sizeof(uint32_t));

        m->size = 0;
        m->stack_size = 0;
        m->stack = &m->at[m->SIZE];
        return m;
}

#if 0
void *arq_arena_malloc(ArqArena *m, uint32_t const num_of_bytes) {
        if (num_of_bytes == 0) return NULL;

        uint32_t padded_size = ARQ_ARENA_SIZE_OF_PADDING * ((num_of_bytes + ARQ_ARENA_SIZE_OF_PADDING - 1) / ARQ_ARENA_SIZE_OF_PADDING);
        uint32_t const already_used = m->size + m->stack_size;
        printf("padded_size %d, already_used %d\n", padded_size, already_used);
        assert(already_used + padded_size <= m->SIZE && "arq_arena_malloc need more memory");

        uint32_t const begin = m->size;
        m->size += padded_size;
        void *buffer = &m->at[begin];
        assert((uintptr_t)buffer % ARQ_ARENA_SIZE_OF_PADDING == 0 && "buffer does not align");
        return buffer;
}
#else
void *arq_arena_malloc(ArqArena *m, uint32_t const num_of_bytes) {
        if (num_of_bytes == 0) return NULL;

        uint32_t padded_size = ARQ_ARENA_SIZE_OF_PADDING * ((num_of_bytes + ARQ_ARENA_SIZE_OF_PADDING - 1) / ARQ_ARENA_SIZE_OF_PADDING);
        uint32_t const already_used = m->size + m->stack_size;
        // printf("padded_size %d, already_used %d\n", padded_size, already_used);
        assert(already_used + num_of_bytes <= m->SIZE && "arq_arena_malloc need more memory");
        if (already_used + padded_size <= m->SIZE) {
                uint32_t const begin = m->size;
                m->size += padded_size;
                void *buffer = &m->at[begin];
                assert((uintptr_t)buffer % ARQ_ARENA_SIZE_OF_PADDING == 0 && "buffer does not align");
                return buffer;
        } else {
                uint32_t const begin = m->size;
                m->size += num_of_bytes;
                void *buffer = &m->at[begin];
                assert((uintptr_t)buffer % ARQ_ARENA_SIZE_OF_PADDING == 0 && "buffer does not align");
                return buffer;
        }
}
#endif

void *arq_arena_malloc_rest(ArqArena *m, uint32_t const size_of_header, uint32_t const size_of_element, uint32_t *num_of_elements) {
        assert(size_of_element > 0);
        uint32_t const size = (m->SIZE - m->size - m->stack_size);
        assert(size >= size_of_element && "size >= size_of_element arq_arena need more memory");
        *num_of_elements = (size - size_of_header) / size_of_element;
        void *buffer = arq_arena_malloc(m, size);
        return buffer;
}


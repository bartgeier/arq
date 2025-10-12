#include "arq_arena.h"
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

Arq_Arena *arq_arena_init(void *buffer, uint32_t const _size) {
        uint32_t const offset = (uintptr_t)buffer % ARQ_ARENA_SIZE_OF_PADDING;
        uint32_t const padding = offset > 0 ? ARQ_ARENA_SIZE_OF_PADDING - offset : 0;
        uint32_t const size = _size - padding;
        uint32_t const header_size = offsetof(Arq_Arena, at);
        Arq_Arena *m = (Arq_Arena *)((char*)buffer + padding);
        assert(_size > padding);
        assert(size > header_size);
        assert((uintptr_t)m % ARQ_ARENA_SIZE_OF_PADDING == 0 && "buffer does not align");
        m->SIZE = size - header_size;
        m->size = 0;
        m->at[0] = 0;
        return m;
}

void *arq_arena_malloc(Arq_Arena *m, uint32_t const num_of_bytes) {
        uint32_t const padded_size = ARQ_ARENA_SIZE_OF_PADDING * ((num_of_bytes + ARQ_ARENA_SIZE_OF_PADDING - 1) / ARQ_ARENA_SIZE_OF_PADDING);

        if (num_of_bytes == 0) return NULL;
        assert(m->size + num_of_bytes <= m->SIZE && "arq_arena_malloc need more memory");

        if (m->size + padded_size <= m->SIZE) {
                uint32_t const begin = m->size;
                void *buffer = &m->at[begin];
                m->size += padded_size;
                assert((uintptr_t)buffer % ARQ_ARENA_SIZE_OF_PADDING == 0 && "buffer does not align");
                return buffer;
        } else {
                uint32_t const begin = m->size;
                void *buffer = &m->at[begin];
                m->size += num_of_bytes;
                assert((uintptr_t)buffer % ARQ_ARENA_SIZE_OF_PADDING == 0 && "buffer does not align");
                return buffer;
        }
}

void *arq_arena_malloc_rest(Arq_Arena *m, uint32_t const size_of_header, uint32_t const size_of_element, uint32_t *num_of_elements) {
        uint32_t const size = (m->SIZE - m->size);
        assert(size_of_element > 0);
        assert(size >= size_of_element && "size >= size_of_element arq_arena need more memory");
        *num_of_elements = (size - size_of_header) / size_of_element;
        return arq_arena_malloc(m, size);
}


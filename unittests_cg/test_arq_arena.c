#include <stddef.h>
#include "cgtest/cgtest.h"
#include "arq_arena.h"
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

static arq_uint32_t padding(arq_uint32_t const offset) {
        return (offset%ARQ_ARENA_SIZE_OF_PADDING > 0)
        ? ARQ_ARENA_SIZE_OF_PADDING - offset%ARQ_ARENA_SIZE_OF_PADDING 
        : 0;
}

void test_arq_arena_init(void) {
        arq_uint32_t offset;
        for (offset = 0; offset < 73; offset++) {
                char array[100] = {0};

                char *buffer = &array[offset];
                Arq_Arena *arena = arq_arena_init(buffer, sizeof(array) - offset);
                arq_uint32_t const size_of_header = offsetof(Arq_Arena, at);

                EXPECT_EQ_INT((char*)arena, (char*)(buffer + padding(offset)));
                EXPECT_EQ_UINT((arq_uint32_t)size_of_header, (arq_uint32_t)8);
                EXPECT_EQ_UINT(arena->SIZE, sizeof(array) - offset - padding(offset) - size_of_header);
                EXPECT_EQ_UINT(arena->size, (arq_uint32_t)0);
        }
}
void test_arq_arena_malloc(void) {
        {
                arq_uint32_t i;
                char array[100] = {0};
                Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
                arq_uint32_t const size_of_header = offsetof(Arq_Arena, at);
                EXPECT_EQ_UINT(size_of_header, (arq_uint32_t)8);
                for (i = 1; i < 10; i++) {
                        (void)arq_arena_malloc(arena, 5);
                        EXPECT_EQ_UINT(arena->SIZE, (arq_uint32_t)100 - size_of_header);
                        EXPECT_EQ_UINT(arena->size, (arq_uint32_t)(i * ARQ_ARENA_SIZE_OF_PADDING));
                }
        }
        {
                arq_uint32_t n;
                for (n = 1; n < 73; n++) {
                        char array[100] = {0};
                        Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
                        arq_uint32_t const size_of_header = offsetof(Arq_Arena, at);
                        EXPECT_EQ_UINT(size_of_header, (arq_uint32_t)8);
                        (void)arq_arena_malloc(arena, n);
                        EXPECT_EQ_UINT(arena->SIZE, (arq_uint32_t)100 - size_of_header);
                        EXPECT_EQ_UINT(arena->size, (arq_uint32_t)(ARQ_ARENA_SIZE_OF_PADDING * ((n + ARQ_ARENA_SIZE_OF_PADDING - 1) / ARQ_ARENA_SIZE_OF_PADDING)));
                }
        }
        {
                char array[100] = {0};
                Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
                arq_uint32_t const size_of_header = offsetof(Arq_Arena, at);
                EXPECT_EQ_UINT(size_of_header, (arq_uint32_t)8);
                EXPECT_EQ_UINT(arena->SIZE, (arq_uint32_t)sizeof(array) - size_of_header);
                (void)arq_arena_malloc(arena, arena->SIZE);
                EXPECT_EQ_UINT(arena->size, arena->SIZE);
                EXPECT_EQ_UINT(arena->size, (arq_uint32_t)92);
        }
}

void test_arq_arena_malloc_rest(void) {
        char buffer[100] = {0};
        Arq_Arena *arena = arq_arena_init(&buffer, sizeof(buffer));
        arq_uint32_t const size_of_header = offsetof(Arq_Arena, at);
        assert((char *)buffer == (char *)arena);
        EXPECT_EQ_UINT(size_of_header, (arq_uint32_t)8);
        {
                arq_uint32_t NUM_OF_TOKEN;
                arq_uint32_t const offset = 0;
                arq_uint32_t const size_of_element = 1;
                (void)arq_arena_malloc_rest(arena, offset, size_of_element, &NUM_OF_TOKEN);
                EXPECT_EQ_UINT(NUM_OF_TOKEN, (arq_uint32_t)92);
                EXPECT_EQ_UINT(NUM_OF_TOKEN, (arq_uint32_t)sizeof(buffer) - size_of_header);
        }
}

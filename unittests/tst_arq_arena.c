#include "gtest/gtest.h"

#include "arq_arena.h"
#include <stddef.h>
#include <stdio.h>

static uint32_t padding(uint32_t const offset) {
        return (offset%ARQ_ARENA_SIZE_OF_PADDING > 0)
        ? ARQ_ARENA_SIZE_OF_PADDING - offset%ARQ_ARENA_SIZE_OF_PADDING 
        : 0;
}

TEST(arq_arena, init) {
        for (uint32_t offset = 0; offset < 73; offset++) {
                char array[100] = {0};

                char *buffer = &array[offset];
                ArqArena *arena = arq_arena_init(buffer, sizeof(array) - offset);
                uint32_t const size_of_header = offsetof(ArqArena, at);

                EXPECT_EQ((char*)arena, (char*)(buffer + padding(offset)));
                EXPECT_EQ((uint32_t)size_of_header, (uint32_t)24);
                EXPECT_EQ(arena->SIZE, sizeof(array) - offset - padding(offset) - size_of_header);
                EXPECT_EQ(arena->stack_size, (uint32_t)0);
                EXPECT_EQ(arena->size, (uint32_t)0);
                // EXPECT_EQ(arena->SIZE, (uint32_t)60);
        }
}

TEST(arq_arena, malloc) {
        {
                char array[100] = {0};
                ArqArena *arena = arq_arena_init(&array, sizeof(array));
                uint32_t const size_of_header = offsetof(ArqArena, at);
                EXPECT_EQ(size_of_header, (uint32_t)24);
                for (uint32_t i = 1; i < 10; i++) {
                        (void)arq_arena_malloc(arena, 5);
                        EXPECT_EQ(arena->SIZE, (uint32_t)100 - size_of_header);
                        EXPECT_EQ(arena->stack_size, (uint32_t)0);
                        EXPECT_EQ(arena->size, (uint32_t)(i * ARQ_ARENA_SIZE_OF_PADDING));
                }
        }
        {
                for (uint32_t n = 1; n < 73; n++) {
                        char array[100] = {0};
                        ArqArena *arena = arq_arena_init(&array, sizeof(array));
                        uint32_t const size_of_header = offsetof(ArqArena, at);
                        EXPECT_EQ(size_of_header, (uint32_t)24);
                        (void)arq_arena_malloc(arena, n);
                        EXPECT_EQ(arena->SIZE, (uint32_t)100 - size_of_header);
                        EXPECT_EQ(arena->stack_size, (uint32_t)0);
                        EXPECT_EQ(arena->size, (uint32_t)(ARQ_ARENA_SIZE_OF_PADDING * ((n + ARQ_ARENA_SIZE_OF_PADDING - 1) / ARQ_ARENA_SIZE_OF_PADDING)));
                }
        }
}

// typedef struct {
//         uint32_t const SIZE;
//         uint32_t size;
//         uint32_t stack_size;
//         char *stack;
//         char at[];
// } ArqArena;

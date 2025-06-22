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
        {
                char array[100] = {0};
                uint32_t const offset = 0;

                char *buffer = &array[offset];
                ArqArena *arena = arq_arena_init(buffer, sizeof(array) - offset);
                uint32_t const size_of_header = offsetof(ArqArena, at);

                EXPECT_EQ((char*)arena, (char*)(buffer + padding(offset)));
                EXPECT_EQ((uint32_t)size_of_header, (uint32_t)24);
                EXPECT_EQ(arena->SIZE, sizeof(array) - offset - padding(offset) - size_of_header);
                EXPECT_EQ(arena->SIZE, (uint32_t)76);
        }
        {
                char array[100] = {0};
                uint32_t const offset = 7;

                char *buffer = &array[offset];
                ArqArena *arena = arq_arena_init(buffer, sizeof(array) - offset);
                uint32_t const size_of_header = offsetof(ArqArena, at);

                EXPECT_EQ((char*)arena, (char*)(buffer + padding(offset)));
                EXPECT_EQ((uint32_t)size_of_header, (uint32_t)24);
                EXPECT_EQ(arena->SIZE, sizeof(array) - offset - padding(offset) - size_of_header);
                EXPECT_EQ(arena->SIZE, (uint32_t)68);
        }
        {
                char array[100] = {0};
                uint32_t const offset = 8;

                char *buffer = &array[offset];
                ArqArena *arena = arq_arena_init(buffer, sizeof(array) - offset);
                uint32_t const size_of_header = offsetof(ArqArena, at);

                EXPECT_EQ((char*)arena, (char*)(buffer + padding(offset)));
                EXPECT_EQ((uint32_t)size_of_header, (uint32_t)24);
                EXPECT_EQ(arena->SIZE, sizeof(array) - offset - padding(offset) - size_of_header);
                EXPECT_EQ(arena->SIZE, (uint32_t)68);
        }
        {
                char array[100] = {0};
                uint32_t const offset = 15;

                char *buffer = &array[offset];
                ArqArena *arena = arq_arena_init(buffer, sizeof(array) - offset);
                uint32_t const size_of_header = offsetof(ArqArena, at);

                EXPECT_EQ((char*)arena, (char*)(buffer + padding(offset)));
                EXPECT_EQ((uint32_t)size_of_header, (uint32_t)24);
                EXPECT_EQ(arena->SIZE, sizeof(array) - offset - padding(offset) - size_of_header);
                EXPECT_EQ(arena->SIZE, (uint32_t)60);
        }
        for (uint32_t i = 0; i < 60; i++) {
                char array[100] = {0};
                uint32_t const offset = i;

                char *buffer = &array[offset];
                ArqArena *arena = arq_arena_init(buffer, sizeof(array) - offset);
                uint32_t const size_of_header = offsetof(ArqArena, at);

                EXPECT_EQ((char*)arena, (char*)(buffer + padding(offset)));
                EXPECT_EQ((uint32_t)size_of_header, (uint32_t)24);
                EXPECT_EQ(arena->SIZE, sizeof(array) - offset - padding(offset) - size_of_header);
                EXPECT_EQ(arena->SIZE, (uint32_t)60);
        }
}


// typedef struct {
//         uint32_t const SIZE;
//         uint32_t size;
//         uint32_t stack_size;
//         char *stack;
//         char at[];
// } ArqArena;

#include "gtest/gtest.h"
#include "arq.h"
#include "arq_queue.h"
#include "arq_arena.h"

TEST(arq_queue, arq_queue_malloc) {
        char array[100] = {0};
        Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
        EXPECT_EQ((void *)arena, (void *)&array[0]);
        uint32_t const arena_size_of_header = offsetof(Arq_Arena, at);
        EXPECT_EQ(arena_size_of_header, (uint32_t)8);
        EXPECT_EQ(arena->SIZE, (uint32_t)100 - arena_size_of_header);

        Arq_Queue *queue = arq_queue_malloc(arena);
        EXPECT_EQ(arena->size, (uint32_t)arena->SIZE);

        EXPECT_EQ((void *)&arena->at[0], (void *)queue);
        uint32_t const queue_size_of_header = offsetof(Arq_Queue, at);
        EXPECT_EQ(queue_size_of_header, (uint32_t)16);

        EXPECT_EQ(queue->NUM_OF_ARGUMENTS, (uint32_t)4);
        EXPECT_EQ(sizeof(Arq_Argument), (uint32_t)16);

        EXPECT_EQ(queue_size_of_header, (uint32_t)16);
        EXPECT_EQ(arena_size_of_header, (uint32_t)8);
        EXPECT_EQ(arena_size_of_header + queue_size_of_header + queue->NUM_OF_ARGUMENTS * sizeof(Arq_Argument), (uint32_t)88);
        EXPECT_TRUE(true);
}

TEST(arq_queue, push_and_pop) {
        char array[100] = {0};
        Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
        Arq_Queue *queue = arq_queue_malloc(arena);
        EXPECT_EQ(queue->read_idx, (uint32_t)0);
        EXPECT_EQ(queue->read_idx, queue->write_idx);
        arq_push_uint8_t(queue, (uint8_t)69);
        EXPECT_EQ(arq_uint8_t(queue), 69);

        arq_push_uint8_t(queue, (uint8_t)1);
        arq_push_uint8_t(queue, (uint8_t)2);
        EXPECT_EQ(arq_uint8_t(queue), 1);
        arq_push_uint8_t(queue, (uint8_t)3);
        EXPECT_EQ(arq_uint8_t(queue), 2);
        EXPECT_EQ(arq_uint8_t(queue), 3);
        EXPECT_EQ(queue->read_idx, queue->write_idx);
}

#include "cgtest/cgtest.h"
#include "arq_main.h"
#include "arq_queue.h"
#include "arq_arena.h"
#include "arq_conversion.h"

void test_arq_queue_arq_queue_malloc(void) {
        char array[100] = {0};
        Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
        EXPECT_EQ_UINT((void *)arena, (void *)&array[0]);
        arq_uint32_t const arena_size_of_header = offsetof(Arq_Arena, at);
        EXPECT_EQ_UINT(arena_size_of_header, (arq_uint32_t)8);
        EXPECT_EQ_UINT(arena->SIZE, (arq_uint32_t)100 - arena_size_of_header);

        Arq_Queue *queue = arq_queue_malloc(arena);
        EXPECT_EQ_UINT(arena->size, (arq_uint32_t)arena->SIZE);

        EXPECT_EQ_UINT((void *)&arena->at[0], (void *)queue);
        arq_uint32_t const queue_size_of_header = offsetof(Arq_Queue, at);
        EXPECT_EQ_UINT(queue_size_of_header, (arq_uint32_t)16);

        EXPECT_EQ_UINT(queue->NUM_OF_ARGUMENTS, (arq_uint32_t)4);
        EXPECT_EQ_UINT(sizeof(Arq_Argument), (arq_uint32_t)16);

        EXPECT_EQ_UINT(queue_size_of_header, (arq_uint32_t)16);
        EXPECT_EQ_UINT(arena_size_of_header, (arq_uint32_t)8);
        EXPECT_EQ_UINT(arena_size_of_header + queue_size_of_header + queue->NUM_OF_ARGUMENTS * sizeof(Arq_Argument), (arq_uint32_t)88);
        EXPECT_TRUE(ARQ_TRUE);
}

void test_arq_queue_push_and_pop(void) {
        union_o x;
        char array[100] = {0};
        Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
        Arq_Queue *queue = arq_queue_malloc(arena);
        EXPECT_EQ_UINT(queue->read_idx, (arq_uint32_t)0);
        EXPECT_EQ_UINT(queue->read_idx, queue->write_idx);
        x.ou.u = 69;
        arq_push_uint(queue, &x);
        EXPECT_EQ_UINT(arq_uint(queue), (arq_uint32_t)69);

        x.ou.u = 1;
        arq_push_uint(queue, &x);
        x.ou.u = 2;
        arq_push_uint(queue, &x);
        EXPECT_EQ_UINT(arq_uint(queue), (arq_uint32_t)1);
        x.ou.u = 3;
        arq_push_uint(queue, &x);
        EXPECT_EQ_UINT(arq_uint(queue), (arq_uint32_t)2);
        EXPECT_EQ_UINT(arq_uint(queue), (arq_uint32_t)3);
        EXPECT_EQ_UINT(queue->read_idx, queue->write_idx);
}

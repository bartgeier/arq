#include "gtest/gtest.h"
#include "arq_queue.h"
#include "arq_arena.h"

TEST(arq_queue,sx) {
        char array[100] = {0};
        Arq_Arena *arena = arq_arena_init(&array, sizeof(array));
        uint32_t const size_of_header = offsetof(Arq_Arena, at);
        EXPECT_EQ(size_of_header, (uint32_t)24);
        EXPECT_EQ(arena->SIZE, (uint32_t)100 - size_of_header);

        Arq_Queue *queue = arq_queue_malloc(arena);
        uint32_t const queue_size_of_header = offsetof(Arq_Queue, at);
        EXPECT_EQ(queue_size_of_header, (uint32_t)16);

        EXPECT_EQ(queue->NUM_OF_ARGUMENTS, (uint32_t)3);
        EXPECT_EQ(sizeof(Arq_Argument), (uint32_t)16);

hier gehts weiter Warnungen eliminieren!
        EXPECT_TRUE(true);

}

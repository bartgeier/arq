#include "arq_queue.h"
#include "arq.h"
#include "arq_symbols.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>

Arq_Queue *arq_queue_malloc(Arq_Arena *arena) {
        uint32_t NUM_OF_ARGUMENTS;
        uint32_t const shrink_snapshot = arena->size;
        Arq_Queue *queue = (Arq_Queue *)arq_arena_malloc_rest(arena, offsetof(Arq_Queue, at), sizeof(Arq_Argument), &NUM_OF_ARGUMENTS);
        queue->shrink = shrink_snapshot;
        queue->NUM_OF_ARGUMENTS = NUM_OF_ARGUMENTS;
        queue->read_idx = 0;
        queue->write_idx = 0; 
        {
                Arq_Argument a;
                a.type_id = ARQ_TYPE_UINT;
                a.value.u32 = 0;
                queue->at[0] = a;
        }
        return queue;
}

void arq_queue_clear(Arq_Queue *queue) {
        queue->read_idx = 0;
        queue->write_idx = 0;
}

static Arq_Argument pop(Arq_Queue *queue) {
        assert(queue->read_idx < queue->write_idx && "queue is empty");
        assert(queue->read_idx < queue->NUM_OF_ARGUMENTS);
        {
                Arq_Argument argument = queue->at[queue->read_idx];
                queue->read_idx++;
                return argument;
        }
}

static void push(Arq_Queue *queue, Arq_Argument const *argument) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        queue->at[queue->write_idx] = *argument;
        queue->write_idx++;
}

void arq_unused(Arq_Queue *queue) {
        (void)pop(queue);
}

uint32_t arq_uint32_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_TYPE_UINT);
        return t.value.u32;
}

uint32_t arq_array_size(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_TYPE_ARRAY_SIZE);
        return t.value.u32;
}

#if 0
uint64_t arq_uint64_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_OPT_UINT64_T);
        return t.value.u64;
}
#endif

int32_t arq_int32_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_TYPE_INT);
        return t.value.i32;
}

double arq_float(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_TYPE_FLOAT);
        return t.value.f;
}

char const *arq_cstr_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_TYPE_CSTR);
        return t.value.cstr;
}

void arq_push_uint32_t(Arq_Queue *queue, uint32_t n) {
        Arq_Argument a;
        a.type_id = ARQ_TYPE_UINT;
        a.value.u32 = n;
        push(queue, &a);
}

uint32_t *arq_push_array_size(Arq_Queue *queue, uint32_t n) {
        Arq_Argument a;
        a.type_id = ARQ_TYPE_ARRAY_SIZE;
        a.value.u32 = n;
        push(queue, &a);
        return &queue->at[queue->write_idx - 1].value.u32;
}

#if 0
void arq_push_uint64_t(Arq_Queue *queue, uint64_t n) {
        Arq_Argument a;
        a.type_id = ARQ_OPT_UINT64_T;
        a.value.u64 = n;
        push(queue, &a);
}
#endif

void arq_push_int32_t(Arq_Queue *queue, int32_t n) {
        Arq_Argument a;
        a.type_id = ARQ_TYPE_INT;
        a.value.i32 = n;
        push(queue, &a);
}

void arq_push_float(Arq_Queue *queue, double f) {
        Arq_Argument a;
        a.type_id = ARQ_TYPE_FLOAT;
        a.value.f = f;
        push(queue, &a);
}

void arq_push_cstr_t(Arq_Queue *queue, char const * cstr) {
        Arq_Argument a;
        a.type_id = ARQ_TYPE_CSTR;
        a.value.cstr = cstr;
        push(queue, &a);
}

#include "arq_queue.h"
#include "arq.h"
#include "arq_symbols.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>



Arq_Queue *arq_queue_malloc(Arq_Arena *arena) {
        uint32_t NUM_OF_ARGUMENTS;
        uint32_t const shrink = arena->size;
        Arq_Queue *queue = (Arq_Queue *)arq_arena_malloc_rest(arena, offsetof(Arq_Queue, at), sizeof(Arq_Argument), &NUM_OF_ARGUMENTS);
        Arq_Argument dummy = {
                .type_id = ARQ_PARA_UINT16_T,
                .u16 = 0,
        };
        Arq_Queue a = {
                .shrink = shrink,
                .NUM_OF_ARGUMENTS = NUM_OF_ARGUMENTS,
                .read_idx = 0,
                .write_idx = 0,
                .at = {dummy}
        };
        memcpy(queue, &a, sizeof(Arq_Queue));
        return queue;
}

static Arq_Argument pop(Arq_Queue *queue) {
        assert(queue->read_idx != queue->write_idx && "queue is empty");
        assert(queue->read_idx < queue->NUM_OF_ARGUMENTS);
        Arq_Argument argument = queue->at[queue->read_idx];
        queue->read_idx++;
        if (queue->read_idx == queue->NUM_OF_ARGUMENTS) {
                queue->read_idx = 0;
        }
        return argument;
}

static void push(Arq_Queue *queue, Arq_Argument const *argument) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        queue->at[queue->write_idx] = *argument;
        queue->write_idx++;
        if (queue->write_idx == queue->NUM_OF_ARGUMENTS) {
                queue->write_idx = 0;
        }
        assert(queue->write_idx != queue->read_idx && "queue ringbuffer overflow");
}

void arq_unused(Arq_Queue *queue) {
        (void)pop(queue);
}

uint8_t arq_uint8_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_UINT8_T);
        return t.u8;
}

uint16_t arq_uint16_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_UINT16_T);
        return t.u16;
}

uint32_t arq_uint32_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_UINT32_T);
        return t.u32;
}

uint64_t arq_uint64_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_UINT64_T);
        return t.u64;
}

int8_t arq_int8_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_INT8_T);
        return t.i8;
}

int16_t arq_int16_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_INT16_T);
        return t.i16;
}

int32_t arq_int32_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_INT32_T);
        return t.i32;
}

char const *arq_cstr_t(Arq_Queue *queue) {
        Arq_Argument t = pop(queue);
        assert(t.type_id == ARQ_PARA_CSTR_T);
        return t.cstr;
}

void arq_push_uint8_t(Arq_Queue *queue, uint8_t n) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_UINT8_T,
                .u8 = n,
        };
        push(queue, &a);
}

void arq_push_uint16_t(Arq_Queue *queue, uint16_t n) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_UINT16_T,
                .u16 = n,
        };
        push(queue, &a);
}

void arq_push_uint32_t(Arq_Queue *queue, uint32_t n) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_UINT32_T,
                .u32 = n,
        };
        push(queue, &a);
}

void arq_push_uint64_t(Arq_Queue *queue, uint64_t n) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_UINT64_T,
                .u64 = n,
        };
        push(queue, &a);
}

void arq_push_int8_t(Arq_Queue *queue, int8_t n) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_INT8_T,
                .i8 = n,
        };
        push(queue, &a);
}

void arq_push_int16_t(Arq_Queue *queue, int16_t n) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_INT16_T,
                .i16 = n,
        };
        push(queue, &a);
}

void arq_push_int32_t(Arq_Queue *queue, int32_t n) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_INT32_T,
                .i32 = n,
        };
        push(queue, &a);
}

void arq_push_cstr_t(Arq_Queue *queue, char const * cstr) {
        Arq_Argument a = {
                .type_id = ARQ_PARA_CSTR_T,
                .cstr = cstr,
        };
        push(queue, &a);
}

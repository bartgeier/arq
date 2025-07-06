#include "arq_queue.h"
#include "arq.h"
#include "arq_symbols.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>

typedef struct {
        Arq_TypeID type_id;
        union {
                uint8_t u8;
                uint16_t u16;
                uint32_t u32;
                uint64_t u64;
                int8_t i8;
                int16_t i16;
                int32_t i32;
                int64_t i64;
                char const *cstr;
        };
} Argument;

typedef struct {
        uint32_t const shrink;
        uint32_t const NUM_OF_ARGUMENTS;
        uint32_t read_idx;
        uint32_t write_idx;
        Argument at[1];
} ArqQueue;

static ArqQueue *queue = NULL;

uint32_t arq_queue_malloc(ArqArena *arena) {
        uint32_t NUM_OF_ARGUMENTS;
        uint32_t const shrink = arena->size;
        ArqQueue *q = arq_arena_malloc_rest(arena, offsetof(ArqQueue, at), sizeof(Argument), &NUM_OF_ARGUMENTS);
        ArqQueue a = {
                .shrink = shrink,
                .NUM_OF_ARGUMENTS = NUM_OF_ARGUMENTS,
                .read_idx = 0,
                .write_idx = 0,
                .at = {{0}}
        };
        memcpy(q, &a, sizeof(ArqQueue));
        queue = q;
        return NUM_OF_ARGUMENTS;
}

void arq_unused(void) {
        assert(queue->read_idx < queue->write_idx);
        queue->read_idx++;
}

uint8_t arq_uint8_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_UINT8_T);
        return t.u8;
}

uint16_t arq_uint16_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_UINT16_T);
        return t.u16;
}

uint32_t arq_uint32_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_UINT32_T);
        return t.u32;
}

uint64_t arq_uint64_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_UINT64_T);
        return t.u64;
}

int8_t arq_int8_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_INT8_T);
        return t.i8;
}

int16_t arq_int16_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_INT16_T);
        return t.i16;
}

int32_t arq_int32_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_INT32_T);
        return t.i32;
}

char const *arq_cstr_t(void) {
        assert(queue->read_idx < queue->write_idx);
        Argument t = queue->at[queue->read_idx];
        queue->read_idx++;
        assert(t.type_id == ARQ_PARA_CSTR_T);
        return t.cstr;
}

void arq_queue_push_uint8_t(uint8_t n) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_UINT8_T,
                .u8 = n,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}

void arq_queue_push_uint16_t(uint16_t n) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_UINT16_T,
                .u16 = n,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}

void arq_queue_push_uint32_t(uint32_t n) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_UINT32_T,
                .u32 = n,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}


void arq_queue_push_uint64_t(uint64_t n) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_UINT64_T,
                .u64 = n,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}


void arq_queue_push_int8_t(int8_t n) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_INT8_T,
                .i8 = n,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}


void arq_queue_push_int16_t(int16_t n) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_INT16_T,
                .i16 = n,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}


void arq_queue_push_int32_t(int32_t n) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_INT32_T,
                .i32 = n,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}

void arq_queue_push_cstr_t(char const * cstr) {
        assert(queue->write_idx < queue->NUM_OF_ARGUMENTS);
        Argument t = {
                .type_id = ARQ_PARA_CSTR_T,
                .cstr = cstr,
        };
        queue->at[queue->write_idx] = t;
        queue->write_idx++;
}

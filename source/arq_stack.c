#include "arq_stack.h"
#include "arq.h"
#include "arq_symbols.h"
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
        };
} Stack_Type;

typedef struct {
        uint32_t const SIZE;
        uint32_t read_idx;
        uint32_t write_idx;
        Stack_Type *at;
} Stack;
static Stack stack = {0};

uint32_t arq_stack_init(void *buffer, uint32_t size) {
        Stack a = {
                .read_idx = 0,
                .write_idx = 0,
                .SIZE = size/sizeof(Stack_Type),
                .at = (Stack_Type *)buffer,
        };
        memcpy(&stack, &a, sizeof(Stack));
        return a.SIZE;
}

void arq_unused(void) {
        assert(stack.read_idx < stack.write_idx);
        stack.read_idx++;
}


uint8_t arq_uint8_t(void) {
        assert(stack.read_idx < stack.write_idx);
        Stack_Type t = stack.at[stack.read_idx];
        stack.read_idx++;
        assert(t.type_id == ARQ_PARA_UINT8_T);
        return t.u8;
}

uint16_t arq_uint16_t(void) {
        assert(stack.read_idx < stack.write_idx);
        Stack_Type t = stack.at[stack.read_idx];
        stack.read_idx++;
        assert(t.type_id == ARQ_PARA_UINT16_T);
        return t.u16;
}

uint32_t arq_uint32_t(void) {
        assert(stack.read_idx < stack.write_idx);
        Stack_Type t = stack.at[stack.read_idx];
        stack.read_idx++;
        assert(t.type_id == ARQ_PARA_UINT32_T);
        return t.u32;
}

uint64_t arq_uint64_t(void) {
        assert(stack.read_idx < stack.write_idx);
        Stack_Type t = stack.at[stack.read_idx];
        stack.read_idx++;
        assert(t.type_id == ARQ_PARA_UINT64_T);
        return t.u64;
}

int8_t arq_int8_t(void) {
        assert(stack.read_idx < stack.write_idx);
        Stack_Type t = stack.at[stack.read_idx];
        stack.read_idx++;
        assert(t.type_id == ARQ_PARA_INT8_T);
        return t.i8;
}

int16_t arq_int16_t(void) {
        assert(stack.read_idx < stack.write_idx);
        Stack_Type t = stack.at[stack.read_idx];
        stack.read_idx++;
        assert(t.type_id == ARQ_PARA_INT16_T);
        return t.i16;
}

int32_t arq_int32_t(void) {
        assert(stack.read_idx < stack.write_idx);
        Stack_Type t = stack.at[stack.read_idx];
        stack.read_idx++;
        assert(t.type_id == ARQ_PARA_INT32_T);
        return t.i32;
}

void arq_push_uint8_t(uint8_t n) {
        assert(stack.write_idx < stack.SIZE);
        Stack_Type t = {
                .type_id = ARQ_PARA_UINT8_T,
                .u8 = n,
        };
        stack.at[stack.write_idx] = t;
        stack.write_idx++;
}

void arq_push_uint16_t(uint16_t n) {
        assert(stack.write_idx < stack.SIZE);
        Stack_Type t = {
                .type_id = ARQ_PARA_UINT16_T,
                .u16 = n,
        };
        stack.at[stack.write_idx] = t;
        stack.write_idx++;
}

void arq_push_uint32_t(uint32_t n) {
        assert(stack.write_idx < stack.SIZE);
        Stack_Type t = {
                .type_id = ARQ_PARA_UINT32_T,
                .u32 = n,
        };
        stack.at[stack.write_idx] = t;
        stack.write_idx++;
}


void arq_push_uint64_t(uint64_t n) {
        assert(stack.write_idx < stack.SIZE);
        Stack_Type t = {
                .type_id = ARQ_PARA_UINT64_T,
                .u64 = n,
        };
        stack.at[stack.write_idx] = t;
        stack.write_idx++;
}


void arq_push_int8_t(int8_t n) {
        assert(stack.write_idx < stack.SIZE);
        Stack_Type t = {
                .type_id = ARQ_PARA_INT8_T,
                .i8 = n,
        };
        stack.at[stack.write_idx] = t;
        stack.write_idx++;
}


void arq_push_int16_t(int16_t n) {
        assert(stack.write_idx < stack.SIZE);
        Stack_Type t = {
                .type_id = ARQ_PARA_INT16_T,
                .i16 = n,
        };
        stack.at[stack.write_idx] = t;
        stack.write_idx++;
}


void arq_push_int32_t(int32_t n) {
        assert(stack.write_idx < stack.SIZE);
        Stack_Type t = {
                .type_id = ARQ_PARA_INT32_T,
                .i32 = n,
        };
        stack.at[stack.write_idx] = t;
        stack.write_idx++;
}


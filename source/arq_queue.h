#ifndef ARQ_QUEUE_H
#define ARQ_QUEUE_H

#include "arq.h"
#include "arq_arena.h"
#include "arq_symbols.h"
//#include <stdint.h>

typedef struct {
        Arq_SymbolID type_id;
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
        } value;
} Arq_Argument;

struct Arq_Queue_tag{
        uint32_t const shrink;
        uint32_t const NUM_OF_ARGUMENTS;
        uint32_t read_idx;
        uint32_t write_idx;
        Arq_Argument at[1];
};

// typedef struct Arq_Queue_tag Arq_Queue;

// typedef struct Arq_Queue_tag{
//         uint32_t const shrink;
//         uint32_t const NUM_OF_ARGUMENTS;
//         uint32_t read_idx;
//         uint32_t write_idx;
//         Arq_Argument at[1];
// } Arq_Queue;

#ifdef __cplusplus
extern "C" {
#endif

Arq_Queue *arq_queue_malloc(Arq_Arena *arena);
void arq_queue_clear(Arq_Queue *queue);

void arq_push_uint8_t(Arq_Queue *queue, uint8_t n);
void arq_push_uint16_t(Arq_Queue *queue, uint16_t n);
void arq_push_uint32_t(Arq_Queue *queue, uint32_t n);
uint32_t *arq_push_array_size(Arq_Queue *queue, uint32_t n);
void arq_push_uint64_t(Arq_Queue *queue, uint64_t n);
void arq_push_int8_t(Arq_Queue *queue, int8_t n);
void arq_push_int16_t(Arq_Queue *queue, int16_t n);
void arq_push_int32_t(Arq_Queue *queue, int32_t n);
void arq_push_int64_t(Arq_Queue *queue, int64_t n);
void arq_push_cstr_t(Arq_Queue *queue, char const *cstr);

#ifdef __cplusplus
}
#endif
#endif

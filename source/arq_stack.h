#ifndef ARQ_STACK_H
#define ARQ_STACK_H

#include <stdint.h>

// return max number of possible arguments
uint32_t arq_stack_init(void *buffer, uint32_t size);

void arq_stack_push_uint8_t(uint8_t n);
void arq_stack_push_uint16_t(uint16_t n);
void arq_stack_push_uint32_t(uint32_t n);
void arq_stack_push_uint64_t(uint64_t n);
void arq_stack_push_int8_t(int8_t n);
void arq_stack_push_int16_t(int16_t n);
void arq_stack_push_int32_t(int32_t n);
void arq_stack_push_int64_t(int64_t n);
void arq_stack_push_cstr_t(char const *cstr);

#endif

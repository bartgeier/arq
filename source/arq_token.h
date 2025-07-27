#ifndef ARQ_TOKEN_H
#define ARQ_TOKEN_H

#include <stdint.h>

typedef struct {
        uint32_t id;
        uint32_t size;
        char const *at;
} Arq_Token;

typedef struct {
        uint32_t num_of_token;
        uint32_t idx;
        Arq_Token at[1];
} Arq_Vector;

typedef struct {
        uint32_t num_of_Vec;
        uint32_t row;
        Arq_Vector *at[1];
} Arq_List;

#endif

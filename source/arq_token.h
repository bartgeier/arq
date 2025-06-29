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
        Arq_Token at[1];
} Arq_Vector;

typedef struct {
        uint32_t num_of_tokenVec;
        Arq_Vector *at[1];
} Arq_List_Vector;

#endif

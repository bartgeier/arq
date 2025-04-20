#ifndef ARQ_TOKEN_H
#define ARQ_TOKEN_H

#include <stdint.h>

typedef struct {
        uint32_t id;
        uint32_t size;
        char const *at;
} OToken;

typedef struct {
        uint32_t const NUM_OF_TOKEN;
        uint32_t num_of_token;
        OToken *at;
} OTokenBuilder;

typedef struct {
        uint32_t num_of_token;
        OToken at[];
} OTokenVec;

typedef struct {
        uint32_t num_of_tokenVec;
        OTokenVec *at[];
} List_of_OTokenVec;

#endif

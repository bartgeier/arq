#ifndef ARQ_TOK_H
#define ARQ_TOK_H

#include "arq_msg.h"
#include "arq_token.h"
#include <stdbool.h>
#include <stdint.h>

bool string_eq(Arq_Token *token, char const *cstr);

typedef struct {
        bool error;
        uint32_t u32;
} uint32_to;

uint32_to arq_tok_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
uint32_to arq_tok_pNumber_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
uint32_to arq_tok_uint32_t_to_uint32_t(Arq_Token const *token);
bool arq_is_a_uint32_t(Arq_Token const *token);

#endif


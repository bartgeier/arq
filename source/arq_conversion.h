#ifndef ARQ_TOK_H
#define ARQ_TOK_H

#include "arq_msg.h"
#include "arq_token.h"
#include "arq_bool.h"
#include "arq_int.h"


typedef struct {
        bool error;
        uint32_t u32;
} uint32_to;

typedef struct {
        bool error;
        int32_t i32;
} int32_to;

#ifdef __cplusplus
exter "C" {
#endif

bool string_eq(Arq_Token const *token, char const *cstr);
uint32_to arq_tok_pNumber_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
int32_to arq_tok_pNumber_to_int32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);

#ifdef __cplusplus
}
#endif
#endif


#ifndef ARQ_TOK_H
#define ARQ_TOK_H

#include "arq_msg.h"
#include "arq_token.h"
#include "arq_bool.h"
#include "arq_int.h"


typedef struct {
        bool error;
        uint32_t u;
} uint_o;

typedef struct {
        bool error;
        int32_t i;
} int_o;

typedef struct {
        bool error;
        double f;
} float_o;

#ifdef __cplusplus
extern "C" {
#endif

bool token_long_option_eq(Arq_Token const *token, char const *cstr);

uint_o arq_tok_pDec_to_uint(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
int_o arq_tok_sDec_to_int(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
uint_o arq_tok_hex_to_uint(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);

float_o arq_tok_decFloat_to_float(Arq_Token const *token);
float_o arq_tok_hexFloat_to_float(Arq_Token const *token);

#ifdef __cplusplus
}
#endif
#endif


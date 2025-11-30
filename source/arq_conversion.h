#ifndef ARQ_TOK_H
#define ARQ_TOK_H

#include "arq_msg.h"
#include "arq_token.h"
#include "arq_bool.h"
#include "arq_int.h"


typedef struct {
        bool error;
        uint8_t u8;
} uint8_to;

typedef struct {
        bool error;
        uint16_t u16;
} uint16_to;

typedef struct {
        bool error;
        uint32_t u32;
} uint32_to;

typedef struct {
        bool error;
        int8_t i8;
} int8_to;

typedef struct {
        bool error;
        int16_t i16;
} int16_to;

typedef struct {
        bool error;
        int32_t i32;
} int32_to;

typedef struct {
        bool error;
        double f;
} float_to;

#ifdef __cplusplus
extern "C" {
#endif

bool token_long_option_eq(Arq_Token const *token, char const *cstr);
uint8_to arq_tok_pDec_to_uint8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
uint16_to arq_tok_pDec_to_uint16_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
uint32_to arq_tok_pDec_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);

int8_to arq_tok_sDec_to_int8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
int16_to arq_tok_sDec_to_int16_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
int32_to arq_tok_sDec_to_int32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);

uint8_to arq_tok_hex_to_uint8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
uint16_to arq_tok_hex_to_uint16_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);
uint32_to arq_tok_hex_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr);

float_to arq_tok_hexFloat_to_float(Arq_Token const *token);

#ifdef __cplusplus
}
#endif
#endif


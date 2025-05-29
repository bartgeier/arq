#ifndef ARQ_STRING_H
#define ARQ_STRING_H

#include "arq_token.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
        uint32_t SIZE; //sizeof(error_buffer),
        uint32_t size;
        char *at;
} Error_msg;
void Error_msg_format(Error_msg *m);
void Error_msg_append_lf(Error_msg *m);
void Error_msg_append_chr(Error_msg *m, char chr);
void Error_msg_append_cstr(Error_msg *m, char const *cstr);
void Error_msg_append_token(Error_msg *m, Arq_Token const *token);

bool string_eq(Arq_Token *token, char const *cstr);

typedef struct {
        bool error;
        uint32_t u32;
} uint32_to;
uint32_to string_to_uint32_t(Arq_Token const *token, Error_msg *error_msg, char const *cstr);
uint32_to p_number_to_uint32_t(Arq_Token const *token, Error_msg *error_msg, char const *cstr);
uint32_to uint32_t_str_to_uint32_t(Arq_Token const *token);
bool is_a_uint32_t_number(Arq_Token const *token);


#endif

#ifndef ARQ_STRING_H
#define ARQ_STRING_H

#include "arq_token.h"
#include <stdbool.h>
#include <stdint.h>

bool string_eq(Arq_Token *token, char const *cstr);

uint32_t string_to_uint32_safe(Arq_Token const *token, bool *overflow);
bool is_a_uint32_t_number(Arq_Token const *token);
uint32_t string_to_uint32(Arq_Token const *token);

#endif

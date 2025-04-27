#ifndef ARQ_STRING_H
#define ARQ_STRING_H

#include "arq_token.h"
#include <stdbool.h>
#include <stdint.h>

bool is_a_uint32_t_number(Arq_Token const *token);
uint32_t string_to_uint32_safe(Arq_Token const *token, bool *overflow);

#endif

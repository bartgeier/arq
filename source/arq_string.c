#include "arq_string.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

bool string_eq(Arq_Token *token, char const *cstr) {
        if (strlen(cstr) != token->size) {
                return false;
        }
        for (uint32_t i = 0; i < token->size; i++) {
                if (cstr[i] != token->at[i]) {
                        return false;
                }
        }
        return true;
}

uint32_t string_to_uint32_safe(Arq_Token const *token, bool *overflow) {
        if (token->id != ARQ_P_NUMBER) {
                printf("%s token id %d wrong!\n", __func__, token->id);
                *overflow = true; 
                return 0;
        }
        uint32_t result = 0;
        for (uint32_t i = 0; i < token->size; i++) {
                uint32_t digit = token->at[i] - '0';
                if (result > (UINT32_MAX - digit) / 10) {
                        *overflow = true;
                        return 0;
                }
                result = result * 10 + digit;
        } 
        *overflow = false;
        return result;
}

uint32_t string_to_uint32(Arq_Token const *token) {
        bool overflow;
        uint32_t result = string_to_uint32_safe(token, &overflow);
        assert(false == overflow);
        return result;
}

bool is_a_uint32_t_number(Arq_Token const *token) {
        bool overflow;
        (void) string_to_uint32_safe(token, &overflow);
        return !overflow;
}

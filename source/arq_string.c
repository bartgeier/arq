#include "arq_string.h"
#include "arq_symbols.h"

bool is_a_uint32_t_number(Arq_Token const *token) {
        if (token->id != ARQ_PARA_P_NUMBER) {
                return false;
        }
        uint32_t result = 0;
        for (uint32_t i = 0; i < token->size; i++) {
                uint32_t digit = token->at[i] - '0';
                if (result > (UINT32_MAX - digit) / 10) {
                        return false;
                }
                result = result * 10 + digit;
        } 
        return true;
}

uint32_t string_to_uint32_safe(Arq_Token const *token, bool *overflow) {
        if (token->id != ARQ_PARA_P_NUMBER) {
                *overflow = true; 
                return false;
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


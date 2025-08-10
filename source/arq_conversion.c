#include "arq_conversion.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>


bool string_eq(Arq_Token const *token, char const *cstr) {
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

uint32_to arq_tok_pNumber_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        assert(token->id == ARQ_P_NUMBER);
        uint32_to result = {0};
        for (uint32_t i = 0; i < token->size; i++) {
                uint32_t digit = token->at[i] - '0';
                if (result.u32 > (UINT32_MAX - digit) / 10) {
                        if (error_msg != NULL) {
                                char buffer[12];
                                sprintf(buffer, "%" PRIu32, UINT32_MAX);
                                arq_msg_clear(error_msg);
                                arq_msg_append_cstr(error_msg, cstr);
                                arq_msg_append_cstr(error_msg, "Token '");
                                arq_msg_append_str(error_msg, token->at, token->size);
                                arq_msg_append_cstr(error_msg, "' positive number > UINT32_MAX ");
                                arq_msg_append_cstr(error_msg, buffer);
                                arq_msg_append_lf(error_msg);
                        }
                        result.error = true;
                        return result;
                }
                result.u32 = result.u32 * 10 + digit;
        } 
        return result;
}

bool arq_is_a_uint32_t(Arq_Token const *token) {
        if (token->id != ARQ_P_NUMBER) {
                return false;
        }
        uint32_to num = arq_tok_pNumber_to_uint32_t(token, NULL, "");
        return !num.error;
}

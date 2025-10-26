#include "arq_conversion.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <arq_inttypes.h>

bool string_eq(Arq_Token const *token, char const *cstr) {
        uint32_t i;
        if (strlen(cstr) != token->size) {
                return false;
        }
        for (i = 0; i < token->size; i++) {
                if (cstr[i] != token->at[i]) {
                        return false;
                }
        }
        return true;
}

uint32_to arq_tok_pNumber_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint32_to result = {0};
        uint32_t i;
        assert(token->id == ARQ_P_NUMBER);
        if (token->at[0] == '+') {
                i = 1;
        } else {
                i = 0;
        }
        for (; i < token->size; i++) {
                uint32_t digit = token->at[i] - '0';
                if (result.u32 > (UINT32_MAX - digit) / 10) {
                        if (error_msg != NULL) {
                                Arq_Token tok = *token;
                                char buffer[12];
                                sprintf(buffer, "%" PRIu32, UINT32_MAX);
                                arq_msg_clear(error_msg);
                                arq_msg_append_cstr(error_msg, cstr);
                                arq_msg_append_cstr(error_msg, "Token '");
                                arq_msg_append_str(error_msg, tok.at, tok.size);
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

int32_to arq_tok_pNumber_to_int32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        int32_to result = {0};
        int32_t SIGN;
        uint32_t i;
        assert(token->id == ARQ_P_NUMBER || token->id == ARQ_N_NUMBER);

        if (token->at[0] == '-') {
                SIGN = -1;
                i = 1;
        } else if (token->at[0] == '+') {
                SIGN = 1;
                i = 1;
        } else {
                SIGN = 1;
                i = 0;
        }
        assert(SIGN != 0);

        for (; i < token->size; i++) {
                char const ch = token->at[i];
                int32_t const digit = ch - '0';

                if (SIGN > 0) {
                        if (result.i32 > (INT32_MAX - digit) / 10) {
                                result.error = true;
                                if (error_msg != NULL) {
                                        Arq_Token tok = *token;
                                        char buffer[12];
                                        sprintf(buffer, "%" PRId32, INT32_MAX);
                                        arq_msg_clear(error_msg);
                                        arq_msg_append_cstr(error_msg, cstr);
                                        arq_msg_append_cstr(error_msg, "Token '");
                                        arq_msg_append_str(error_msg, tok.at, tok.size);
                                        arq_msg_append_cstr(error_msg, "' positive number > INT32_MAX ");
                                        arq_msg_append_cstr(error_msg, buffer);
                                        arq_msg_append_lf(error_msg);
                                }
                                return result;
                        }
                        result.i32 = result.i32 * 10 + digit;
                } else {
                        if (result.i32 < (INT32_MIN + digit) / 10) {
                                result.error = true;
                                if (error_msg != NULL) {
                                        Arq_Token tok = *token;
                                        char buffer[12];
                                        sprintf(buffer, "%" PRId32, INT32_MIN);
                                        arq_msg_clear(error_msg);
                                        arq_msg_append_cstr(error_msg, cstr);
                                        arq_msg_append_cstr(error_msg, "Token '");
                                        arq_msg_append_str(error_msg, tok.at, tok.size);
                                        arq_msg_append_cstr(error_msg, "' negative number < INT32_MIN ");
                                        arq_msg_append_cstr(error_msg, buffer);
                                        arq_msg_append_lf(error_msg);
                                }
                                return result;
                        }
                        result.i32 = result.i32 * 10 - digit;
                }
        }
        return result;
}

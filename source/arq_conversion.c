#include "arq_conversion.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <arq_inttypes.h>

bool token_long_option_eq(Arq_Token const *token, char const *cstr) {
        uint32_t i;
        if (strlen(cstr) != token->size - 2) {
                return false;

        }
        for (i = 2; i < token->size; i++) {
                if (cstr[i - 2] != token->at[i]) {
                        return false;
                }
        }
        return true;
}

uint32_to arq_tok_pDec_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint32_to result = {0};
        uint32_t i;
        assert(token->id == ARQ_P_DEC);
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

uint8_to arq_tok_pDec_to_uint8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint8_to result;
        uint32_to num = arq_tok_pDec_to_uint32_t(token, error_msg, cstr);
        if (num.u32 > UINT8_MAX || num.error) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRIu8, UINT8_MAX);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' positive number > UINT8_MAX ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        }
        result.u8 = (uint8_t) num.u32;
        result.error = num.error;
        return result;
}

int32_to arq_tok_sDec_to_int32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        int32_to result = {0};
        int32_t SIGN;
        uint32_t i;
        assert(token->id == ARQ_P_DEC || token->id == ARQ_N_DEC);

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

int8_to arq_tok_sDec_to_int8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        int8_to result;
        int32_to num = arq_tok_sDec_to_int32_t(token, error_msg, cstr);
        if (num.i32 > INT8_MAX || (num.i32 > 0 && num.error)) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRId8, INT8_MAX);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' positive number > INT8_MAX ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        } else if (num.i32 < INT8_MIN || (num.i32 < 0 && num.error)) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRId8, INT8_MIN);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' negative number < INT8_MIN ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        }
        result.i8 = (uint8_t) num.i32;
        result.error = num.error;
        return result;
}

uint8_to arq_tok_hex_to_uint8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint8_to result;
        uint32_to num = arq_tok_hex_to_uint32_t(token, error_msg, cstr);
        if (num.u32 > UINT8_MAX || num.error) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' more than 2 hex digits");
                        arq_msg_append_lf(error_msg);
                }
        }
        result.u8 = (uint8_t) num.u32;
        result.error = num.error;
        return result;
}

uint32_to arq_tok_hex_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint32_to result = {0};
        uint32_t i;
        assert(token->id == ARQ_HEX);
        for (i = 2; i < token->size; i++) {
                char const ch = token->at[i];
                uint32_t digit = 0;
                switch(ch) {
                case '0': case '1': case '2': case '3': case '4': 
                case '5': case '6': case '7': case '8': case '9':
                        digit = ch - '0';
                        break;
                case 'A': case 'a':
                        digit = 10;
                        break;
                case 'B': case 'b':
                        digit = 11;
                        break;
                case 'C': case 'c':
                        digit = 12;
                        break;
                case 'D': case 'd':
                        digit = 13;
                        break;
                case 'E': case 'e':
                        digit = 14;
                        break;
                case 'F': case 'f':
                        digit = 15;
                        break;
                default:
                        assert(false);
                }
                if (result.u32 > (UINT32_MAX - digit) / 10) {
                        result.error = true;
                        if (error_msg != NULL) {
                                Arq_Token tok = *token;
                                arq_msg_clear(error_msg);
                                arq_msg_append_cstr(error_msg, cstr);
                                arq_msg_append_cstr(error_msg, "Token '");
                                arq_msg_append_str(error_msg, tok.at, tok.size);
                                arq_msg_append_cstr(error_msg, "' more than 8 hex digits");
                                arq_msg_append_lf(error_msg);
                        }
                        return result;
                }
                result.u32 = result.u32 * 16 + digit;
        }
        return result;
}

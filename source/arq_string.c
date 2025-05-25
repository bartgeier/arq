#include "arq_string.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void Error_msg_append_lf(Error_msg *m) {
        assert(1 < m->SIZE - 1 - m->size);
        m->at[m->size++] = '\n';
        m->at[m->size] = 0;
}

void Error_msg_append_cstr(Error_msg *m, char const *cstr) {
        uint32_t len = strlen(cstr);
        assert(len < m->SIZE - 1 - m->size);
        for (uint32_t i = 0; i < len; i++) {
                m->at[m->size++] = cstr[i];
        }
        m->at[m->size] = 0;
}

void Error_msg_append_token(Error_msg *m, Arq_Token const *t) {
        assert(t->size < m->SIZE - 1 - m->size);
        for (uint32_t i = 0; i < t->size; i++) {
                m->at[m->size++] = t->at[i];
        }
        m->at[m->size] = 0;
}

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

uint32_to p_number_to_uint32_t(Arq_Token const *token, Error_msg *error_msg, char const *cstr) {
        assert(token->id == ARQ_P_NUMBER);
        uint32_to result = {0};
        for (uint32_t i = 0; i < token->size; i++) {
                uint32_t digit = token->at[i] - '0';
                if (result.u32 > (UINT32_MAX - digit) / 10) {
                        if (error_msg != NULL) {
                                Error_msg_append_cstr(error_msg, cstr);
                                Error_msg_append_cstr(error_msg, "Token '");
                                Error_msg_append_token(error_msg, token);
                                Error_msg_append_cstr(error_msg, "' positive number > UINT32_MAX! ");
                                Error_msg_append_cstr(error_msg, __func__);
                                Error_msg_append_lf(error_msg);
                        }
                        result.error = true;
                        return result;
                }
                result.u32 = result.u32 * 10 + digit;
        } 
        return result;
}


 uint32_to string_to_uint32_t(Arq_Token const *token, Error_msg *error_msg, char const *cstr) {
        uint32_to result = {0};
        if (token->id != ARQ_P_NUMBER) {
                if (error_msg != NULL) {
                        Error_msg_append_cstr(error_msg, cstr);
                        Error_msg_append_cstr(error_msg, "Token '");
                        Error_msg_append_token(error_msg, token);
                        Error_msg_append_cstr(error_msg, "' is not a positiv number! ");
                        Error_msg_append_cstr(error_msg, __func__);
                        Error_msg_append_lf(error_msg);
                }
                result.error = true;
                return result;
        }
        result = p_number_to_uint32_t(token, error_msg, cstr);
        return result;
}

uint32_to uint32_t_str_to_uint32_t(Arq_Token const *token) {
        uint32_to num = p_number_to_uint32_t(token, NULL, "");
        assert(num.error == false);
        return num;
}

bool is_a_uint32_t_number(Arq_Token const *token) {
        uint32_to num = string_to_uint32_t(token, NULL, "");
        return !num.error;
}

#include "arq_string.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

void Error_msg_format(Error_msg *m) {
        uint32_t number_of_lf = 0;
        for (size_t i = 0; i < m->size; i++) {
                uint32_t const last = m->size - 1;
                if (m->at[i] == '\n' && i < last) {
                        number_of_lf++;
                }
        }
        uint32_t const indent_size = 4;
        uint32_t const shift_right = number_of_lf * indent_size;
        assert(m->size + shift_right < m->SIZE);
        for (uint32_t i = 0; i < m->size; i++) {
                m->at[m->size - 1 - i + shift_right] = m->at[m->size - 1 - i];
        }
        m->size += shift_right;
        m->at[m->size] = '\0';
        uint32_t j = 0;
        for (uint32_t i = shift_right; i < m->size; i++) {
                m->at[j++] = m->at[i];
                if (m->at[i] == '\n' && i + indent_size < m->size) {
                        memset(&m->at[j], ' ', indent_size);
                        j += indent_size;
                }
        }
}

void Error_msg_append_chr(Error_msg *m, char chr) {
        assert(m->size + 1 < m->SIZE);
        m->at[m->size++] = chr;
        m->at[m->size] = 0; // thats wy m->size has to be smaller than m->SIZE
}

void Error_msg_append_lf(Error_msg *m) {
        Error_msg_append_chr(m, '\n');
}

void Error_msg_append_cstr(Error_msg *m, char const *cstr) {
        uint32_t len = strlen(cstr);
        for (uint32_t i = 0; i < len; i++) {
                Error_msg_append_chr(m, cstr[i]);
        }
}

void Error_msg_append_token(Error_msg *m, Arq_Token const *t) {
        for (uint32_t i = 0; i < t->size; i++) {
                Error_msg_append_chr(m, t->at[i]);
        }
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
                                char buffer[12];
                                sprintf(buffer, "%" PRIu32, UINT32_MAX);
                                Error_msg_append_cstr(error_msg, cstr);
                                Error_msg_append_cstr(error_msg, "Token '");
                                Error_msg_append_token(error_msg, token);
                                Error_msg_append_cstr(error_msg, "' positive number > UINT32_MAX ");
                                Error_msg_append_cstr(error_msg, buffer);
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
                        Error_msg_append_cstr(error_msg, "' is not a positiv number");
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

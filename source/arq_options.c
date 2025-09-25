#include "arq_options.h"
#include "arq_symbols.h"
#include "arq_immediate.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
    uint32_t id;
    char *at;
} KeyWord;

static KeyWord const key_words[] = {
        {  ARQ_OPT_NULL,  "NULL" },
        {  ARQ_OPT_CSTR_T,  "cstr_t" },
        {  ARQ_OPT_UINT8_T,  "uint8_t" },
        {  ARQ_OPT_UINT16_T, "uint16_t" },
        {  ARQ_OPT_UINT32_T, "uint32_t" },
        {  ARQ_OPT_UINT64_T, "uint64_t" },
        {  ARQ_OPT_INT8_T,   "int8_t" },
        {  ARQ_OPT_INT16_T,  "int16_t" },
        {  ARQ_OPT_INT32_T,  "int32_t" },
        {  ARQ_OPT_INT64_T,  "int64_t" },
};

static bool str_eq_keyword(char const *str, uint32_t const str_size, KeyWord const *cstr) {
        if (str_size != strlen(cstr->at)) {
                return false;
        }
        for (uint32_t i = 0; i < str_size; i++) {
                if (str[i] != cstr->at[i]) {
                        return false;
                }
        }
        return true;
}

static bool is_identifier_start(char chr) {
        return isalpha(chr) || chr == '_';
}

static bool is_identifier(char chr) {
        return isalnum(chr) || chr == '_';
}

typedef struct {
        uint32_t cursor_idx;
        uint32_t SIZE;
        char const *at;
} Lexer;

static void skip_space(Lexer *l) {
    while (l->cursor_idx < l->SIZE && (l->at[l->cursor_idx] == 0 || isspace(l->at[l->cursor_idx]))) {
            l->cursor_idx++;
    }
}

static Arq_Token next_token(Lexer *l) {
        Arq_Token t = {0};
        skip_space(l);
        t.at = l->at;
        if (l->cursor_idx == l->SIZE ) {
                // space tail
                t.id = ARQ_OPT_NO_TOKEN;
                t.at = &l->at[l->cursor_idx];
                t.size = 0;
                return t;
        }

        if (l->at[l->cursor_idx] == '=') {
                t.id = ARQ_OPT_EQ; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == ',') {
                t.id = ARQ_OPT_COMMA; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == '(') {
                t.id = ARQ_OPT_L_PARENTHESIS; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == ')') {
                t.id = ARQ_OPT_R_PARENTHESIS; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == 0) {
                t.id = ARQ_OPT_TERMINATOR; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (is_identifier_start(l->at[l->cursor_idx])) {
                t.id = ARQ_OPT_IDENTFIER; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                while (l->cursor_idx < l->SIZE && is_identifier(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                for (uint32_t i = 0; i < sizeof(key_words)/sizeof(KeyWord); i++) {
                        if (str_eq_keyword(t.at,t.size, &key_words[i])) {
                                t.id = key_words[i].id;
                        }
                }
                return t;
        }

        if (isdigit(l->at[l->cursor_idx])) {
                t.id = ARQ_P_NUMBER; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                return t;
        }

        if (l->at[l->cursor_idx] == '-') {
                t.id = ARQ_N_NUMBER; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                return t;
        }

        if (l->cursor_idx < l->SIZE) {
                t.id = ARQ_OPT_UNKNOWN; 
                t.at = &l->at[l->cursor_idx];
                t.size = 0;
                while (l->cursor_idx < l->SIZE && !isspace(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
        }
        return t;
}

uint32_t arq_option_parameter_idx(Arq_Option const *option) {
        uint32_t result = 0;
        if (option->chr != 0) {
                result += 3;
        }
        uint32_t sl = strlen(option->name);
        if (sl > 0) {
                result += sl + 3;
        }
        return result;
}

uint32_to arq_option_verify_vector(Arq_OptVector *tokens, Arq_msg *error_msg) {
        tokens->idx = 0;
        char *error_str = "' missing open parenthesis '('\n";
        if (arq_imm_L_parenthesis(tokens)) {
                while (tokens->idx < tokens->num_of_token) {
                        if (arq_imm_type(tokens, ARQ_OPT_UINT32_T)) {
                                if (arq_imm_not_identifier(tokens)) {
                                        error_str = "' is not a parameter name\n";
                                        break; // error

                                }
                                if (arq_imm_equal(tokens)) {
                                        if (false == arq_imm_is_a_uint32_t(tokens)) {
                                                error_str = "' is not a positive number\n";
                                                break; // error
                                        }
                                        error_str = "' but expected ',' or ')'\n";
                                } else {
                                        error_str = "' but expected ',' or '=' or ')'\n";
                                }
                                if (arq_imm_comma(tokens)) {
                                        continue;
                                }
                                goto next_argument;
                        }

                        if (arq_imm_type(tokens, ARQ_OPT_CSTR_T)) {
                                if (arq_imm_not_identifier(tokens)) {
                                        error_str = "' is not a parameter name\n";
                                        break; // error

                                }
                                if (arq_imm_equal(tokens)) {
                                        if (false == arq_imm_is_a_NULL(tokens)) {
                                                error_str =  "' must be NULL\n";
                                                break; // error 
                                        }
                                        error_str = "' but expected ',' or ')'\n";
                                } else {
                                        error_str = "' but expected ',' or '=' or ')'\n";
                                }
                                if (arq_imm_comma(tokens)) {
                                        continue;
                                }
                                goto next_argument;
                        } 

                        error_str = "' is not a type\n";
        next_argument:
                        if (arq_imm_R_parenthesis(tokens)) {
                                error_str = "' after ')' no tokens allowed!\n";
                                if (arq_imm_terminator(tokens)) {
                                        uint32_to const idx = { .error = false, .u32 = 0 };
                                        return idx;
                                } 
                        }
                        break; // error
                } // while
        }

        // error
        uint32_to const idx = { .error = true, .u32 = tokens->at[tokens->idx].at - tokens->at[0].at };
        Arq_Token token = tokens->at[tokens->idx];
        arq_msg_clear(error_msg);
        arq_msg_append_cstr(error_msg, "Option failure:\n");
        arq_msg_append_cstr(error_msg, "token '");
        arq_msg_append_str(error_msg, token.at, token.size);
        arq_msg_append_cstr(error_msg, error_str);
        return idx;
}


void arq_option_tokenize(Arq_Option const *option, Arq_OptVector *v, uint32_t const NUM_OF_TOKEN) {
        v->num_of_token = 0;
        v->idx = 0;
        uint32_t len = strlen(option->arguments);
        Lexer l = {
                .cursor_idx = 0,
                .SIZE = len,
                .at = option->arguments,
        };
        while (l.cursor_idx < l.SIZE) {
                assert(v->num_of_token < NUM_OF_TOKEN);
                Arq_Token t = next_token(&l);
                if (t.id == ARQ_OPT_NO_TOKEN) continue;
                v->at[v->num_of_token++] = t;
        }
        Arq_Token t = { .id = ARQ_OPT_TERMINATOR, .at = &l.at[l.SIZE], .size = 0 };
        assert(v->num_of_token < NUM_OF_TOKEN);
        v->at[v->num_of_token++] = t;
}


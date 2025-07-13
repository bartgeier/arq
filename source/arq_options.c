#include "arq_options.h"
#include "arq_symbols.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

typedef struct {
    uint32_t id;
    char *at;
} KeyWord;

static KeyWord const key_words[] = {
        {  ARQ_PARA_NULL,  "NULL" },
        {  ARQ_PARA_CSTR_T,  "cstr_t" },
        {  ARQ_PARA_UINT8_T,  "uint8_t" },
        {  ARQ_PARA_UINT16_T, "uint16_t" },
        {  ARQ_PARA_UINT32_T, "uint32_t" },
        {  ARQ_PARA_UINT64_T, "uint64_t" },
        {  ARQ_PARA_INT8_T,   "int8_t" },
        {  ARQ_PARA_INT16_T,  "int16_t" },
        {  ARQ_PARA_INT32_T,  "int32_t" },
        {  ARQ_PARA_INT64_T,  "int64_t" },
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
                t.id = ARQ_PARA_SPACE_TAIL; 
                t.at = &l->at[l->cursor_idx];
                t.size = 0;
                return t;
        }

        if (l->at[l->cursor_idx] == '=') {
                t.id = ARQ_PARA_EQ; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == ',') {
                t.id = ARQ_PARA_COMMA; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == 0) {
                t.id = ARQ_END; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (is_identifier_start(l->at[l->cursor_idx])) {
                t.id = ARQ_PARA_IDENTFIER; 
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
                t.id = ARQ_PARA_UNKNOWN; 
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

uint32_to arq_option_verify_vector(Arq_Vector const *tokens, Arq_msg *error_msg) {
        uint32_t i = 0;
        while (i < tokens->num_of_token) {
                if (tokens->at[i].id == ARQ_PARA_UINT32_T) {
                        i++;
                        if (tokens->at[i].id == ARQ_PARA_EQ) {
                                i++;
                                if (arq_is_a_uint32_t(&tokens->at[i])) {
                                        i++;
                                        if (tokens->at[i].id == ARQ_PARA_COMMA) {
                                                i++;
                                                continue;
                                        } else if (tokens->at[i].id == ARQ_END) {
                                                i++;
                                                break;
                                        } else {
                                                arq_msg_append_cstr(error_msg, "Assert Option:\n");
                                                arq_msg_append_cstr(error_msg, "token '");
                                                arq_msg_append_str(error_msg, tokens->at[i].at, tokens->at[i].size);
                                                arq_msg_append_cstr(error_msg, "' but expected ',' or ''\n");
                                                uint32_to const idx = { .error = true, .u32 = tokens->at[i].at - tokens->at[0].at };
                                                return idx;
                                        }
                                } else {
                                        arq_msg_append_cstr(error_msg, "Assert Option:\n");
                                        arq_msg_append_cstr(error_msg, "token '");
                                        arq_msg_append_str(error_msg, tokens->at[i].at, tokens->at[i].size);
                                        arq_msg_append_cstr(error_msg, "' is not a positive number\n");
                                        uint32_to const idx = { .error = true, .u32 = tokens->at[i].at - tokens->at[0].at };
                                        return idx;
                                }
                        } else if (tokens->at[i].id == ARQ_PARA_COMMA) {
                                i++;
                                continue;
                        } else if (tokens->at[i].id == ARQ_END) {
                                i++;
                                break;
                        } else {
                                arq_msg_append_cstr(error_msg, "Assert Option:\n");
                                arq_msg_append_cstr(error_msg, "token '");
                                arq_msg_append_str(error_msg, tokens->at[i].at, tokens->at[i].size);
                                arq_msg_append_cstr(error_msg, "' but expected ',' or '=' or ''\n");
                                uint32_to const idx = { .error = true, .u32 = tokens->at[i].at - tokens->at[0].at };
                                return idx;
                        }
                } else if (tokens->at[i].id == ARQ_PARA_CSTR_T) {
                        i++;
                        if (tokens->at[i].id == ARQ_PARA_EQ) {
                                i++;
                                if (tokens->at[i].id == ARQ_PARA_NULL) {
                                        i++;
                                        if (tokens->at[i].id == ARQ_PARA_COMMA) {
                                                i++;
                                                continue;
                                        } else if (tokens->at[i].id == ARQ_END) {
                                                i++;
                                                break;
                                        } else {
                                                arq_msg_append_cstr(error_msg, "Assert Option:\n");
                                                arq_msg_append_cstr(error_msg, "token '");
                                                arq_msg_append_str(error_msg, tokens->at[i].at, tokens->at[i].size);
                                                arq_msg_append_cstr(error_msg, "' but expected ',' or ''\n");
                                                uint32_to const idx = { .error = true, .u32 = tokens->at[i].at - tokens->at[0].at };
                                                return idx;
                                        }
                                } else {
                                        arq_msg_append_cstr(error_msg, "Assert Option:\n");
                                        arq_msg_append_cstr(error_msg, "token '");
                                        arq_msg_append_str(error_msg, tokens->at[i].at, tokens->at[i].size);
                                        arq_msg_append_cstr(error_msg, "' must be NULL\n");
                                        uint32_to const idx = { .error = true, .u32 = tokens->at[i].at - tokens->at[0].at };
                                        return idx;
                                }

                        } else if (tokens->at[i].id == ARQ_PARA_COMMA) {
                                i++;
                                continue;
                        } else if (tokens->at[i].id == ARQ_END) {
                                i++;
                                break;
                        } else {
                                arq_msg_append_cstr(error_msg, "Assert Option:\n");
                                arq_msg_append_cstr(error_msg, "token '");
                                arq_msg_append_str(error_msg, tokens->at[i].at, tokens->at[i].size);
                                arq_msg_append_cstr(error_msg, "' but expected ',' or '=' or ''\n");
                                uint32_to const idx = { .error = true, .u32 = tokens->at[i].at - tokens->at[0].at };
                                return idx;
                        }

                } else if (tokens->at[i].id == ARQ_END) {
                        break;
                } else {
                        arq_msg_append_cstr(error_msg, "Assert Option:\n");
                        arq_msg_append_cstr(error_msg, "token '");
                        arq_msg_append_str(error_msg, tokens->at[i].at, tokens->at[i].size);
                        arq_msg_append_cstr(error_msg, "' is not a type\n");
                        uint32_to const idx = { .error = true, .u32 = tokens->at[i].at - tokens->at[0].at };
                        return idx;
                }
        }
        uint32_to const idx = { .error = false, .u32 = 0 };
        return idx;
}


void arq_option_tokenize(Arq_Option const *option, Arq_Vector *v, uint32_t const NUM_OF_TOKEN) {
        assert(v->num_of_token == 0);
        uint32_t len = strlen(option->arguments);
        Lexer l = {
                .cursor_idx = 0,
                .SIZE = len,
                .at = option->arguments,
        };
        while (l.cursor_idx < l.SIZE) {
                assert(v->num_of_token < NUM_OF_TOKEN);
                Arq_Token t = next_token(&l);
                v->at[v->num_of_token++] = t;
        }
        Arq_Token t = { .id = ARQ_END, .at = &l.at[l.SIZE], .size = 0 };
        assert(v->num_of_token < NUM_OF_TOKEN);
        v->at[v->num_of_token++] = t;
}


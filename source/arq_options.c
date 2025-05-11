#include "arq_options.h"
#include "arq_symbols.h"
#include "arq_string.h"
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

static bool verify_vector(Arq_Vector const *tokens) {
        uint32_t i = 0;
        printf("\n");
        while (i < tokens->num_of_token) {
                if (tokens->at[i].id == ARQ_PARA_UINT32_T) {
                        i++;
                        if (tokens->at[i].id == ARQ_PARA_EQ) {
                                i++;
                                if (is_a_uint32_t_number(&tokens->at[i])) {
                                        i++;
                                        if (tokens->at[i].id == ARQ_PARA_COMMA) {
                                                i++;
                                                continue;
                                        } else if (tokens->at[i].id == ARQ_END) {
                                                i++;
                                                break;
                                        } else {
                                                printf("i = %d %s ',' or end expected\n", i, tokens->at[i].at);
                                                return false;
                                        }
                                } else {
                                        printf("i = %d %s is not a type!\tn", i, tokens->at[i].at);
                                        return false;
                                }
                        } else if (tokens->at[i].id == ARQ_PARA_COMMA) {
                                i++;
                                continue;
                        } else if (tokens->at[i].id == ARQ_END) {
                                i++;
                                break;
                        } else {
                                printf("i = %d %s a ',' or '=' expected.\n", i, tokens->at[i].at);
                                return false;
                        }
                } else if (tokens->at[i].id == ARQ_END) {
                        break;
                } else {
                        printf("i = %d token.id = %d a type expected.\n", i, tokens->at[i].id);
                        return false;
                }
        }
        return true;
}

uint32_t arq_num_of_option_token(Arq_Option const *option) {
        uint32_t len = strlen(option->arguments);
        Lexer l = {
                .cursor_idx = 0,
                .SIZE = len,
                .at = option->arguments,
        };
        uint32_t num_of_token = 0;
        while (l.cursor_idx < l.SIZE) {
                (void) next_token(&l);
                num_of_token++;
        }
        return ++num_of_token;
}

void arq_tokenize_option(Arq_Option const *option, Arq_Vector *v, uint32_t num_of_token) {
        assert(v->num_of_token == 0);
        uint32_t len = strlen(option->arguments);
        Lexer l = {
                .cursor_idx = 0,
                .SIZE = len,
                .at = option->arguments,
        };
        while (l.cursor_idx < l.SIZE) {
                assert(v->num_of_token < num_of_token);
                Arq_Token t = next_token(&l);
                v->at[v->num_of_token++] = t;
        }
        Arq_Token t = { .id = ARQ_END, .at = &l.at[l.SIZE], .size = 0 };
        v->at[v->num_of_token++] = t;

        // for(uint32_t i = 0; i < v->num_of_token; i++) {
        //         printf("i = %d %d a ',' or '=' expected.\n", i, v->at[i].id);
        // }
        // assert(true == verify_vector(v));
         (void) verify_vector(v);
}



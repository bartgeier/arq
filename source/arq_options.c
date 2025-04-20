#include "arq_options.h"
#include "arq_symbols.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

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
                t.id = ARQ_PARA_END; 
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
                return t;
        }

        if (isdigit(l->at[l->cursor_idx])) {
                t.id = ARQ_PARA_P_NUMBER; 
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
                t.id = ARQ_PARA_N_NUMBER; 
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
        return num_of_token;
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
}


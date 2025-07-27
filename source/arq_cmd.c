#include "arq_cmd.h"
#include "arq_symbols.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
        uint32_t cursor_idx;
        uint32_t SIZE;
        char const *at;
} Lexer;

static bool is_long_identifier(char chr) {
        return isalnum(chr) || chr == '-';
}

static bool is_short_identifier(char chr) {
        return isalpha(chr) || chr == '?';
}

static void set_token_RAW_STR(Arq_Token *t, Lexer *l) {
        t->id = ARQ_CMD_RAW_STR;
        t->size = l->SIZE;
        l->cursor_idx = l->SIZE;
}

static bool start_p_number(Arq_Token *t, Lexer *l) {
        if (isdigit(l->at[l->cursor_idx])) {
                t->id = ARQ_P_NUMBER; 
                t->at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t->size = 1;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t->size++;
                }
                if (l->cursor_idx < l->SIZE) {
                        set_token_RAW_STR(t, l);
                }
                return true;
        }
        return false;
}

static bool start_n_number(Arq_Token *t, Lexer *l) {
        if (isdigit(l->at[l->cursor_idx])) {
                t->id = ARQ_N_NUMBER; 
                l->cursor_idx++;
                t->size++;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t->size++;
                }
                if (l->cursor_idx < l->SIZE) {
                        set_token_RAW_STR(t, l);
                }
                return true;
        }
        return false;
}

static Arq_Token next_argument_token(Lexer *l) {
        Arq_Token t = {0};

        if (start_p_number(&t, l)) {
                return t;
        }

        if (l->at[l->cursor_idx] == '-') {
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                if (start_n_number(&t, l)) {
                        return t;
                }
                if (l->at[l->cursor_idx] == '-') {
                        set_token_RAW_STR(&t ,l);
                        if (t.size == 2) {
                                t.id = ARQ_CMD_ARGUMENT_MODE; 
                        }
                        return t; 
                }
                set_token_RAW_STR(&t ,l);
                return t;
        }
        t.at = &l->at[l->cursor_idx]; 
        set_token_RAW_STR(&t ,l);
        return t;
}

static Arq_Token next_token(Lexer *l, bool const bundling) {
        Arq_Token t = {0};

        if (bundling && is_short_identifier(l->at[l->cursor_idx])) {
                t.id = ARQ_CMD_SHORT_OPTION; 
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                return t;
        }

        if (start_p_number(&t, l)) {
                return t;
        }

        if (l->at[l->cursor_idx] == '-') {
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                if (start_n_number(&t, l)) {
                        return t;
                }

                if (is_short_identifier(l->at[l->cursor_idx])) {
                        t.id = ARQ_CMD_SHORT_OPTION; 
                        t.at = &l->at[l->cursor_idx];
                        l->cursor_idx++;
                        t.size = 1;
                        return t;
                }

                if (l->at[l->cursor_idx] == '-') {
                        t.id = ARQ_CMD_LONG_OPTION; 
                        l->cursor_idx++;
                        t.at = &l->at[l->cursor_idx];
                        t.size = 0;
                        while (l->cursor_idx < l->SIZE && is_long_identifier(l->at[l->cursor_idx])) {
                                l->cursor_idx++;
                                t.size++;
                        }
                        if (t.size == 0) {
                                t.at = t.at - 2;
                                t.size = 2;
                                t.id = ARQ_CMD_ARGUMENT_MODE; 
                        }
                        return t; 
                }
                set_token_RAW_STR(&t ,l);
                return t; 
        }

        t.at = &l->at[l->cursor_idx]; 
        set_token_RAW_STR(&t ,l);
        return t;
}

void arq_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t const num_of_token) {
        assert(argc >= 1);
        argv += 1;
        argc -= 1;
        v->num_of_token = 0;
        v->idx = 0;
        bool bundling = false;
        bool option = true;
        for (int i = 0; i < argc; i++) {
                Lexer lexer = {
                        .SIZE = strlen(argv[i]),
                        .cursor_idx = 0,
                        .at = argv[i],
                };
                Arq_Token const t = option
                ? next_token(&lexer, bundling)
                : next_argument_token(&lexer);

                if (t.id == ARQ_CMD_ARGUMENT_MODE) {
                        option = !option;
                } else if (t.id != ARQ_INIT) {
                        assert(v->num_of_token < num_of_token);
                        v->at[v->num_of_token++] = t;
                }
                bundling = lexer.cursor_idx < lexer.SIZE;
                while (bundling) { 
                        // Option clustering
                        lexer.SIZE -= lexer.cursor_idx;
                        lexer.at = &lexer.at[lexer.cursor_idx];
                        lexer.cursor_idx = 0;
                        Arq_Token const t = next_token(&lexer, bundling);
                        assert(v->num_of_token < num_of_token);
                        v->at[v->num_of_token++] = t;
                        bundling = (t.id == ARQ_CMD_SHORT_OPTION)
                        && (lexer.cursor_idx < lexer.SIZE);
                }
        }
        Arq_Token t = {
                .id = ARQ_END,
                .at = NULL,
                .size = 0,
        };
        assert(v->num_of_token < num_of_token);
        v->at[v->num_of_token++] = t;
}


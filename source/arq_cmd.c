#include "arq_cmd.h"
#include "arq_symbols.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
//#include <stdio.h>

typedef struct {
        uint32_t cursor_idx;
        uint32_t SIZE;
        char const *at;
} Lexer;

static bool is_long_identifier(char chr) {
        return isalnum(chr) 
                // || chr == '"'
                || chr == '!'
                || chr == '#'
                || chr == '$'
                || chr == '%'
                || chr == '&'
                || chr == '\''
                || chr == '('
                || chr == ')'
                || chr == '*'
                || chr == '+'
                || chr == ','
                || chr == '-'
                || chr == '.'
                || chr == '/'
                || chr == ':'
                || chr == ';'
                || chr == '<'
                || chr == '='
                || chr == '>'
                || chr == '?'
                || chr == '@'
                || chr == '['
                || chr == '\\'
                || chr == ']'
                || chr == '^'
                || chr == '_'
                || chr == '{'
                || chr == '|'
                || chr == '}'
                || chr == '~';
}

static bool is_short_identifier(char chr) {
        return isalnum(chr) 
                // || chr == '"'
                || chr == '!'
                || chr == '#'
                || chr == '$'
                || chr == '%'
                || chr == '&'
                || chr == '\''
                || chr == '('
                || chr == ')'
                || chr == '*'
                || chr == '+'
                || chr == ','
                //|| chr == '-'
                || chr == '.'
                || chr == '/'
                || chr == ':'
                || chr == ';'
                || chr == '<'
                || chr == '='
                || chr == '>'
                || chr == '?'
                || chr == '@'
                || chr == '['
                || chr == '\\'
                || chr == ']'
                || chr == '^'
                || chr == '_'
                || chr == '{'
                || chr == '|'
                || chr == '}'
                || chr == '~';
}

static Arq_Token next_token(Lexer *l) {
        Arq_Token t = {0};

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
                t.at = &l->at[l->cursor_idx];
                l->cursor_idx++;
                t.size = 1;
                if (isdigit(l->at[l->cursor_idx])) {
                        t.id = ARQ_N_NUMBER; 
                        l->cursor_idx++;
                        t.size++;
                        while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                                l->cursor_idx++;
                                t.size++;
                        }
                        if (l->cursor_idx < l->SIZE) {
                                t.id = ARQ_CMD_ERROR_NOT_NUMBER;
                        }
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
                                t.id = ARQ_CMD_OPTION_END; 
                        }
                        return t; 
                }
                return t; 
        }

        t.id = ARQ_CMD_RAW_STR; 
        t.at = &l->at[l->cursor_idx]; 
        l->cursor_idx++;
        t.size = 1;
        while (l->cursor_idx < l->SIZE) {
                l->cursor_idx++;
                t.size++;
        }
        if (l->cursor_idx < l->SIZE) {
                t.id = ARQ_CMD_ERROR_NOT_IDENTIFIER;
        }
        return t;
}

uint32_t arq_num_of_cmd_token(int argc, char **argv) {
        uint32_t num_of_token = 0;
        if (argc  < 2) {
                return 0;
        }
        argv += 1;
        argc -= 1;
        for (int i = 0; i < argc; i++) {
                Lexer lexer = {
                        .SIZE = strlen(argv[i]),
                        .cursor_idx = 0,
                        .at = argv[i],
                };
                (void) next_token(&lexer);
                num_of_token++;
        }
        return num_of_token;
}

void arq_tokenize_cmd(int argc, char **argv, Arq_Vector *v, uint32_t num_of_token) {
        if (argc  < 2) {
                return;
        }
        argv += 1;
        argc -= 1;
        for (int i = 0; i < argc; i++) {
                Lexer lexer = {
                        .SIZE = strlen(argv[i]),
                        .cursor_idx = 0,
                        .at = argv[i],
                };
                Arq_Token t = next_token(&lexer);
                assert(v->num_of_token < num_of_token);
                v->at[v->num_of_token++] = t;
        }
        Arq_Token t = {
                .id = ARQ_END,
                .at = NULL,
                .size = 0,
        };
        v->at[v->num_of_token++] = t;
}


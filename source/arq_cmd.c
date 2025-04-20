#include "arq_cmd.h"
#include "arq_symbols.h"
#include <stdint.h>
#include <stdbool.h>
//#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#if 0
typedef struct {
        uint32_t id;
        uint32_t size;
        char const *at;
} OToken;

typedef struct {
        uint32_t const NUM_OF_TOKEN;
        uint32_t num_of_token;
        OToken *at;
} OTokenVec;

#endif

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
                t.id = ARQ_CMD_P_NUMBER; 
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
                        t.id = ARQ_CMD_N_NUMBER; 
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

#if 0
static OToken b0[20];
static OTokenVec ts = {
        .NUM_OF_TOKEN = sizeof(b0)/sizeof(OToken),
        .num_of_token = 0,
        .at = &b0[0],
};
#endif

static void append_token(Arq_VectorBuilder *tb, Arq_Token const *t) {
        assert(tb->num_of_token < tb->NUM_OF_TOKEN);
        tb->at[tb->num_of_token++] = *t;
}

void arq_tokenize_cmd(int argc, char **argv, Arq_VectorBuilder *tb) {
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
                append_token(tb, &t);
        }
        return;
}


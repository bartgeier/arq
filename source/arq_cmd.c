#include "arq_cmd.h"
#include "arq_symbols.h"
#include "arq_int.h"
#include "arq_bool.h"
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

static bool hex_start(Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if ((idx + 2 < l->SIZE)
        && (l->at[idx + 0] == '0') 
        && (l->at[idx + 1] == 'x' || l->at[idx + 1] == 'X') 
        && isxdigit(l->at[idx + 2])) {
                l->cursor_idx += 3;
                return true;
        }
        return false;
}

static bool has_hex_exponent(char const s) {
    return (s == 'p') || (s == 'P');
}

static bool p_dec_start(Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if (isdigit(l->at[idx])) {
                l->cursor_idx += 1;
                return true;
        } else if (idx + 1 < l->SIZE 
        && l->at[idx] == '+' 
        && isdigit(l->at[idx + 1])) {
                l->cursor_idx += 2;
                return true;
        }
        return false;
}

static bool n_dec_start(Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if (idx + 1 < l->SIZE 
        && l->at[idx] == '-'
        && isdigit(l->at[idx + 1])) {
                l->cursor_idx += 2;
                return true;
        }
        return false;
}

static bool start_short_identifier(Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && is_short_identifier(l->at[l->cursor_idx + 1])) {
                l->cursor_idx += 2;
                return true;
        }
        return false;
}

static bool start_long_identifier(Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && l->at[l->cursor_idx + 1] == '-'
        && is_long_identifier(l->at[l->cursor_idx + 2])) {
                l->cursor_idx += 3;
                return true;
        }
        return false;
}

static bool start_dash_dash(Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && l->at[l->cursor_idx + 1] == '-'
        && l->SIZE == 2) {
                l->cursor_idx += 2;
                return true;
        }
        return false;
}

static Arq_Token next_token(Lexer *l, bool const bundling) {
        Arq_Token t = {0};
        t.at = &l->at[l->cursor_idx];

        if (bundling && is_short_identifier(l->at[l->cursor_idx])) {
                t.id = ARQ_CMD_SHORT_OPTION; 
                l->cursor_idx++;
                t.size = 1;
                return t;
        }

        if (hex_start(l)) {
                t.id = ARQ_HEX;
                t.size = &l->at[l->cursor_idx] - t.at;
                while (l->cursor_idx < l->SIZE && isxdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                if (l->cursor_idx < l->SIZE && ('.' == l->at[l->cursor_idx])) {
                        t.id = ARQ_CMD_RAW_STR;
                        l->cursor_idx++;
                        t.size++;
                        while (l->cursor_idx < l->SIZE && isxdigit(l->at[l->cursor_idx])) {
                                l->cursor_idx++;
                                t.size++;
                        }
                        if (l->cursor_idx < l->SIZE && has_hex_exponent(l->at[l->cursor_idx])) {
                                l->cursor_idx++;
                                t.size++;
                                if (p_dec_start(l) || n_dec_start(l)) {
                                        t.id = ARQ_HEX_FLOAT; 
                                        t.size = &l->at[l->cursor_idx] - t.at;
                                        while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                                                l->cursor_idx++;
                                                t.size++;
                                        }
                                        if (l->cursor_idx == l->SIZE) { return t; }
                                }
                        } 
                } else if (l->cursor_idx < l->SIZE && has_hex_exponent(l->at[l->cursor_idx])) {
                        t.id = ARQ_CMD_RAW_STR;
                        l->cursor_idx++;
                        t.size++;
                        if (p_dec_start(l) || n_dec_start(l)) {
                                t.id = ARQ_HEX_FLOAT; 
                                t.size = &l->at[l->cursor_idx] - t.at;
                                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                                        l->cursor_idx++;
                                        t.size++;
                                }
                                if (l->cursor_idx == l->SIZE) { return t; }
                        }
                } else { 
                        if (l->cursor_idx == l->SIZE) { return t; }
                }
        }
hier gehts weiter dec float 
        if (p_dec_start(l)) {
                t.id = ARQ_P_DEC; 
                t.size = &l->at[l->cursor_idx] - t.at;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                if (l->cursor_idx == l->SIZE) {
                        return t;
                }
        }

        if (n_dec_start(l)) {
                t.id = ARQ_N_DEC; 
                t.size = &l->at[l->cursor_idx] - t.at;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                if (l->cursor_idx == l->SIZE) {
                        return t;
                }
        }

        if (start_short_identifier(l)) {
                t.id = ARQ_CMD_SHORT_OPTION; 
                t.size = &l->at[l->cursor_idx] - t.at;
                return t;
        }

        if (start_long_identifier(l)) {
                t.id = ARQ_CMD_LONG_OPTION;
                t.size = &l->at[l->cursor_idx] - t.at;
                while (l->cursor_idx < l->SIZE && is_long_identifier(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                if (l->cursor_idx == l->SIZE) {
                        return t;
                }
        }

        if (start_dash_dash(l)) {
                t.id = ARQ_CMD_DASHDASH; 
                t.size = &l->at[l->cursor_idx] - t.at;
                return t;
        }

        set_token_RAW_STR(&t ,l);
        return t;
}

void arq_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t const num_of_token) {
        bool bundling = false;
        int i;
        assert(argc >= 1);
        argv += 1;
        argc -= 1;
        v->num_of_token = 0;
        v->idx = 0;
        for (i = 0; i < argc; i++) {
                Lexer lexer;
                lexer.SIZE = strlen(argv[i]);
                lexer.cursor_idx = 0;
                lexer.at = argv[i];
                assert( v->num_of_token < num_of_token);
                v->at[v->num_of_token++] = next_token(&lexer, bundling);
                bundling = lexer.cursor_idx < lexer.SIZE;
                while (bundling) { 
                        /* Option clustering */
                        lexer.SIZE -= lexer.cursor_idx;
                        lexer.at = &lexer.at[lexer.cursor_idx];
                        lexer.cursor_idx = 0;
                        {
                                Arq_Token const t = next_token(&lexer, bundling);
                                assert(v->num_of_token < num_of_token);
                                v->at[v->num_of_token++] = t;
                                bundling = (t.id == ARQ_CMD_SHORT_OPTION)
                                && (lexer.cursor_idx < lexer.SIZE);
                        }
                }
        }
        {
                Arq_Token t;
                t.id = ARQ_CMD_END_OF_LINE;
                t.size = 0;
                t.at = NULL;
                assert(v->num_of_token < num_of_token);
                v->at[v->num_of_token++] = t;
        }
}


#include "arq_options.h"
#include <string.h>
#include <ctype.h>
#include "arq_bool.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
    uint32_t id;
    char const *at;
} KeyWord;

static KeyWord const key_words[] = {
        {  ARQ_OPT_NULL,     "NULL" },
        {  ARQ_OPT_CSTR_T,   "cstr_t" },
        {  ARQ_OPT_UINT32_T, "uint32_t" },
        {  ARQ_OPT_INT32_T,  "int32_t" },
        {  ARQ_OPT_FLOAT,    "float" },
};

Lexer arq_lexer_create(void) {
        Lexer lexer;
        lexer.cursor_idx = 0;
        lexer.SIZE = 0;
        lexer.at = NULL;
        lexer.token.at = NULL;
        lexer.token.id = 0;
        lexer.token.size = 0;
        return lexer;
}

static bool str_eq_keyword(char const *str, uint32_t const str_size, KeyWord const *cstr) {
        uint32_t i;
        if (str_size != strlen(cstr->at)) {
                return false;
        }
        for (i = 0; i < str_size; i++) {
                if (str[i] != cstr->at[i]) {
                        return false;
                }
        }
        return true;
}

static bool is_identifier(char const chr) {
        return isalnum(chr) || chr == '_';
}

static bool identifier_start(Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if (isalpha(l->at[idx]) || l->at[idx] == '_') {
                l->cursor_idx += 1;
                return true;
        }
        return false;
}

static bool array_start(Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if ((idx + 1 < l->SIZE)
        && (l->at[idx] == '[') 
        && (l->at[idx + 1] == ']')) {
                l->cursor_idx += 2;
                return true; 
        }
        return false;
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

static bool has_dec_exponent(Lexer *l) {
        if (l->cursor_idx + 1 < l->SIZE) { 
                uint32_t const idx = l->cursor_idx;
                char const chr = l->at[l->cursor_idx];
                bool isExp = (chr == 'e') || (chr == 'E');
                l->cursor_idx++;
                isExp &= p_dec_start(l) || n_dec_start(l);
                if (isExp) {
                        return true;
                }
                l->cursor_idx = idx;
        }
        return false;
}

static void dec_float(Lexer *l, Arq_Token *t) {
        if (l->cursor_idx < l->SIZE && ('.' == l->at[l->cursor_idx])) {
                /* fractional part */
                t->id = ARQ_DEC_FLOAT;
                l->cursor_idx++;
                t->size++;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t->size++;
                }
        }
        if (has_dec_exponent(l)) {
                t->id = ARQ_DEC_FLOAT;
                t->size = &l->at[l->cursor_idx] - t->at;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t->size++;
                }
                return;
        }
        return;
}

static void skip_space(Lexer *l) {
    while (l->cursor_idx < l->SIZE && (l->at[l->cursor_idx] == 0 || isspace(l->at[l->cursor_idx]))) {
            l->cursor_idx++;
    }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/* cmd_ */
static bool is_long_identifier(char chr) {
        return isalnum(chr) || chr == '-';
}

static bool is_short_identifier(char chr) {
        return isalpha(chr) || chr == '?';
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

static Arq_Token next_token(Lexer *l) {
        Arq_Token t = {0};
        skip_space(l);
        t.at = &l->at[l->cursor_idx];
        if (l->cursor_idx == l->SIZE ) {
                /* space tail */
                t.id = ARQ_OPT_NO_TOKEN;
                t.size = 0;
                return t;
        }

        if (l->at[l->cursor_idx] == '=') {
                t.id = ARQ_OPT_EQ; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == ',') {
                t.id = ARQ_OPT_COMMA; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == '(') {
                t.id = ARQ_OPT_L_PARENTHESIS; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == ')') {
                t.id = ARQ_OPT_R_PARENTHESIS; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == 0) {
                t.id = ARQ_OPT_TERMINATOR; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }


        if (identifier_start(l)) {
                uint32_t i;
                t.id = ARQ_OPT_IDENTFIER; 
                t.size = &l->at[l->cursor_idx] - t.at;
                while (l->cursor_idx < l->SIZE && is_identifier(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                for (i = 0; i < sizeof(key_words)/sizeof(KeyWord); i++) {
                        if (str_eq_keyword(t.at,t.size, &key_words[i])) {
                                t.id = key_words[i].id;
                        }
                }
                return t;
        }

        if (array_start(l)) {
                t.id = ARQ_OPT_ARRAY; 
                t.size = &l->at[l->cursor_idx] - t.at;
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
                        t.id = ARQ_OPT_NO_TOKEN;
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
                                        return t;
                                }
                        } 
                } else if (l->cursor_idx < l->SIZE && has_hex_exponent(l->at[l->cursor_idx])) {
                        t.id = ARQ_OPT_NO_TOKEN;
                        l->cursor_idx++;
                        t.size++;
                        if (p_dec_start(l) || n_dec_start(l)) {
                                t.id = ARQ_HEX_FLOAT; 
                                t.size = &l->at[l->cursor_idx] - t.at;
                                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                                        l->cursor_idx++;
                                        t.size++;
                                }
                                return t;
                        }
                } else { 
                        return t;
                }
        }

        if (l->at[l->cursor_idx] ==  '.') {
                dec_float(l, &t);
                switch (t.id) {
                case ARQ_DEC_FLOAT: return t;
                default: break;
                }; 
        }

        if (p_dec_start(l)) {
                t.id = ARQ_P_DEC; 
                t.size = &l->at[l->cursor_idx] - t.at;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                dec_float(l, &t);
                switch (t.id) {
                case ARQ_DEC_FLOAT:
                case ARQ_P_DEC: return t;
                default: break;
                }; 
        }

        if (n_dec_start(l)) {
                t.id = ARQ_N_DEC; 
                t.size = &l->at[l->cursor_idx] - t.at;
                while (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
                dec_float(l, &t);
                switch (t.id) {
                case ARQ_DEC_FLOAT:
                case ARQ_N_DEC: return t;
                default: break;
                }; 
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

        if (l->cursor_idx < l->SIZE) {
                t.id = ARQ_OPT_UNKNOWN; 
                t.size = 0;
                while (l->cursor_idx < l->SIZE && !isspace(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
        }
        return t;
}

void arq_option_tokenize(Arq_Option const *option, Arq_OptVector *v, uint32_t const NUM_OF_TOKEN) {
        Lexer l;
        l.cursor_idx = 0;
        l.SIZE = strlen(option->arguments);
        l.at = option->arguments;

        v->num_of_token = 0;
        v->idx = 0;
        while (l.cursor_idx < l.SIZE) {
                assert(v->num_of_token < NUM_OF_TOKEN);
                {
                        Arq_Token t = next_token(&l);
                        if (t.id == ARQ_OPT_NO_TOKEN) continue;
                        v->at[v->num_of_token++] = t;
                }
        }
        {
                Arq_Token t;
                t.id = ARQ_OPT_TERMINATOR; 
                t.at = &l.at[l.SIZE]; 
                t.size = 0;
                assert(v->num_of_token < NUM_OF_TOKEN);
                v->at[v->num_of_token++] = t;
        }
}

void arq_next_opt_token(Lexer *l) {
        l->token = next_token(l);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#if 1
static Arq_Token next_cmd_token(Lexer *lexer) {
        Arq_Token token = next_token(lexer);
        if (token.id == ARQ_CMD_SHORT_OPTION) {
                return token;
        }
        if (lexer->cursor_idx < lexer->SIZE) {
                token.id = ARQ_CMD_RAW_STR;
                token.size = lexer->SIZE;
                lexer->cursor_idx = lexer->SIZE;
        }
        return token;
} 

void arq_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t const num_of_token) {
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
                v->at[v->num_of_token++] = next_cmd_token(&lexer);
                while (lexer.cursor_idx < lexer.SIZE) { 
                        /* bundled optons, Option clustering */
                        lexer.SIZE -= lexer.cursor_idx;
                        lexer.at = &lexer.at[lexer.cursor_idx];
                        lexer.cursor_idx = 0;
                        if (is_short_identifier(lexer.at[lexer.cursor_idx])) {
                                Arq_Token t = {0};
                                t.at = &lexer.at[lexer.cursor_idx];
                                t.id = ARQ_CMD_SHORT_OPTION; 
                                t.size = 1;
                                lexer.cursor_idx++;
                                assert(v->num_of_token < num_of_token);
                                v->at[v->num_of_token++] = t;
                        } else {
                                assert( v->num_of_token < num_of_token);
                                v->at[v->num_of_token++] = next_cmd_token(&lexer);
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
#endif


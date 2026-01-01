#include "arq_lexer.h"
#include "arq_bool.h"
#include "arq_token.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
    uint32_t id;
    char const *at;
} KeyWord;

static KeyWord const key_words[] = {
        {  ARQ_NULL,          "NULL" },
        {  ARQ_TYPE_CSTR,     "cstr_t" },
        {  ARQ_TYPE_UINT,     "uint" },
        {  ARQ_TYPE_INT,      "int" },
        {  ARQ_TYPE_FLOAT,    "float" },
};

Arq_Lexer arq_lexer_create(void) {
        Arq_Lexer lexer;
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

static bool identifier_start(Arq_Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if (isalpha(l->at[idx]) || l->at[idx] == '_') {
                l->cursor_idx += 1;
                return true;
        }
        return false;
}

static bool array_start(Arq_Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if ((idx + 1 < l->SIZE)
        && (l->at[idx] == '[') 
        && (l->at[idx + 1] == ']')) {
                l->cursor_idx += 2;
                return true; 
        }
        return false;
}

static bool hex_start(Arq_Lexer *l) {
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

static bool p_dec_start(Arq_Lexer *l) {
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

static bool n_dec_start(Arq_Lexer *l) {
        uint32_t const idx = l->cursor_idx;
        if (idx + 1 < l->SIZE 
        && l->at[idx] == '-'
        && isdigit(l->at[idx + 1])) {
                l->cursor_idx += 2;
                return true;
        }
        return false;
}

static bool has_dec_exponent(Arq_Lexer *l) {
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

static void dec_float(Arq_Lexer *l, Arq_Token *t) {
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

static void skip_space(Arq_Lexer *l) {
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

static bool start_short_identifier(Arq_Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && is_short_identifier(l->at[l->cursor_idx + 1])) {
                l->cursor_idx += 2;
                return true;
        }
        return false;
}

static bool start_long_identifier(Arq_Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && l->at[l->cursor_idx + 1] == '-'
        && is_long_identifier(l->at[l->cursor_idx + 2])) {
                l->cursor_idx += 3;
                return true;
        }
        return false;
}

static bool start_dash_dash(Arq_Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && l->at[l->cursor_idx + 1] == '-'
        && l->SIZE == 2) {
                l->cursor_idx += 2;
                return true;
        }
        return false;
}

static Arq_Token next_token(Arq_Lexer *l) {
        Arq_Token t = {0};
        skip_space(l);
        t.at = &l->at[l->cursor_idx];
        if (l->cursor_idx == l->SIZE ) {
                /* space tail */
                t.id = ARQ_NO_TOKEN;
                t.size = 0;
                return t;
        }

        if (l->at[l->cursor_idx] == '=') {
                t.id = ARQ_OP_EQ; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == ',') {
                t.id = ARQ_OP_COMMA; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == '(') {
                t.id = ARQ_OP_L_PARENTHESIS; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == ')') {
                t.id = ARQ_OP_R_PARENTHESIS; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }

        if (l->at[l->cursor_idx] == 0) {
                t.id = ARQ_OP_TERMINATOR; 
                l->cursor_idx++;
                t.size = 1;
                return t; 
        }


        if (identifier_start(l)) {
                uint32_t i;
                t.id = ARQ_IDENTFIER; 
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
                t.id = ARQ_OP_ARRAY; 
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
                        t.id = ARQ_NO_TOKEN;
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
                        t.id = ARQ_NO_TOKEN;
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
                t.id = ARQ_OP_UNKNOWN; 
                t.size = 0;
                while (l->cursor_idx < l->SIZE && !isspace(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
        }
        return t;
}

void arq_lexer_next_opt_token(Arq_Lexer *l) {
        l->token = next_token(l);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static Arq_Token next_cmd_token(Arq_Lexer *lexer) {
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

Arq_LexerCmd arq_lexerCmd_create(int argc, char **argv) {
        Arq_Lexer base = arq_lexer_create();
        Arq_LexerCmd l;
        l.lexer = base;
        l.argc = argc - 1;
        l.argv = argv + 1;
        l.argIdx = 0;
        l.bundeling = false;
        return l;
}

void arq_lexerCmd_reset(Arq_LexerCmd *cmd) {
        Arq_Lexer base = arq_lexer_create();
        cmd->lexer = base;
        cmd->argIdx = 0;
        cmd->bundeling = false;
        return;
}

void arq_lexer_next_cmd_token(Arq_LexerCmd *cmd) {
        if (cmd->argIdx >= cmd->argc) {
                cmd->lexer.token.id = ARQ_NO_TOKEN;
                cmd->lexer.SIZE = 0;
                cmd->lexer.cursor_idx = 0;
                cmd->lexer.at = NULL;
                cmd->lexer.token.at = NULL;
                cmd->lexer.token.size = 0;
                cmd->bundeling = false;
                return;
        }

        if (!cmd->bundeling) {
                cmd->lexer.SIZE = strlen(cmd->argv[cmd->argIdx]);
                cmd->lexer.at = cmd->argv[cmd->argIdx];
                cmd->lexer.cursor_idx = 0;
                cmd->lexer.token = next_cmd_token(&cmd->lexer);
        } else if (is_short_identifier(cmd->lexer.at[cmd->lexer.cursor_idx])) {
                /* bundled options, Option clustering */
                cmd->lexer.token.at = &cmd->lexer.at[cmd->lexer.cursor_idx];
                cmd->lexer.token.id = ARQ_CMD_SHORT_OPTION; 
                cmd->lexer.token.size = 1;
                cmd->lexer.cursor_idx++;
        } else {
                cmd->lexer.token = next_cmd_token(&cmd->lexer);
        }

        if (cmd->lexer.cursor_idx < cmd->lexer.SIZE) {
                cmd->bundeling = true;
                return;
        }
        if (cmd->argIdx < cmd->argc) {
                cmd->argIdx++;
        }
        cmd->bundeling = false;
        return;
}

void arq_lexer_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t const num_of_token) {
        Arq_LexerCmd cmd = arq_lexerCmd_create(argc, argv);
        v->num_of_token = 0;
        v->idx = 0;
        do {
                arq_lexer_next_cmd_token(&cmd);
                assert( v->num_of_token < num_of_token);
                v->at[v->num_of_token++] = cmd.lexer.token;
        } while (cmd.lexer.token.id != ARQ_NO_TOKEN);
}

#include "arq_lexer.h"
#include "arq_token.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
    arq_uint32_t id;
    char const *at;
} KeyWord;

static KeyWord const key_words[] = {
        {  ARQ_NULL,          "NULL" },
        {  ARQ_TYPE_CSTR,     "cstr_t" },
        {  ARQ_TYPE_UINT,     "uint" },
        {  ARQ_TYPE_INT,      "int" },
        {  ARQ_TYPE_FLOAT,    "float" },
};


static arq_bool_t str_eq_keyword(char const *str, arq_uint32_t const str_size, KeyWord const *cstr) {
        arq_uint32_t i;
        if (str_size != strlen(cstr->at)) {
                return ARQ_FALSE;
        }
        for (i = 0; i < str_size; i++) {
                if (str[i] != cstr->at[i]) {
                        return ARQ_FALSE;
                }
        }
        return ARQ_TRUE;
}

static arq_bool_t is_identifier(char const chr) {
        return isalnum(chr) || chr == '_';
}

static arq_bool_t identifier_start(Arq_Lexer *l) {
        arq_uint32_t const idx = l->cursor_idx;
        if (isalpha(l->at[idx]) || l->at[idx] == '_') {
                l->cursor_idx += 1;
                return ARQ_TRUE;
        }
        return ARQ_FALSE;
}

static arq_bool_t array_start(Arq_Lexer *l) {
        arq_uint32_t const idx = l->cursor_idx;
        if ((idx + 1 < l->SIZE)
        && (l->at[idx] == '[') 
        && (l->at[idx + 1] == ']')) {
                l->cursor_idx += 2;
                return ARQ_TRUE; 
        }
        return ARQ_FALSE;
}

static arq_bool_t hex_start(Arq_Lexer *l) {
        arq_uint32_t idx = l->cursor_idx;
        if (l->at[idx] == '+' || l->at[idx] == '-') {
                if (idx + 1 == l->SIZE) { 
                        return ARQ_FALSE;
                }
                idx++;
        }
        if (l->at[idx] != '0') {
                return ARQ_FALSE;
        }
        if (idx + 1 == l->SIZE) { 
                return ARQ_FALSE;
        }
        idx++;
        if (l->at[idx] != 'x' && l->at[idx + 1] != 'X') {
                return ARQ_FALSE;
        }
        if (idx + 1 == l->SIZE) { 
                return ARQ_FALSE;
        }
        idx++;
        if (!isxdigit(l->at[idx])) {
                return ARQ_FALSE;
        } 
        l->cursor_idx = idx + 1;
        return ARQ_TRUE;
}

static arq_bool_t has_hex_exponent(char const s) {
    return (s == 'p') || (s == 'P');
}

static arq_bool_t p_dec_start(Arq_Lexer *l) {
        arq_uint32_t const idx = l->cursor_idx;
        if (isdigit(l->at[idx])) {
                l->cursor_idx += 1;
                return ARQ_TRUE;
        } else if (idx + 1 < l->SIZE 
        && l->at[idx] == '+' 
        && isdigit(l->at[idx + 1])) {
                l->cursor_idx += 2;
                return ARQ_TRUE;
        }
        return ARQ_FALSE;
}

static arq_bool_t n_dec_start(Arq_Lexer *l) {
        arq_uint32_t const idx = l->cursor_idx;
        if (idx + 1 < l->SIZE 
        && l->at[idx] == '-'
        && isdigit(l->at[idx + 1])) {
                l->cursor_idx += 2;
                return ARQ_TRUE;
        }
        return ARQ_FALSE;
}

static arq_bool_t has_dec_exponent(Arq_Lexer *l) {
        if (l->cursor_idx + 1 < l->SIZE) { 
                arq_uint32_t const idx = l->cursor_idx;
                char const chr = l->at[l->cursor_idx];
                arq_bool_t isExp = (chr == 'e') || (chr == 'E');
                l->cursor_idx++;
                isExp &= p_dec_start(l) || n_dec_start(l);
                if (isExp) {
                        return ARQ_TRUE;
                }
                l->cursor_idx = idx;
        }
        return ARQ_FALSE;
}
#if 1
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
#else
static void dec_float(Arq_Lexer *l, Arq_Token *t) {
        if (l->cursor_idx < l->SIZE && ('.' == l->at[l->cursor_idx])) {
                /* fractional part */
                l->cursor_idx++;
                t->size++;
                if (l->cursor_idx < l->SIZE && isdigit(l->at[l->cursor_idx])) {
                        t->id = ARQ_DEC_FLOAT;
                }
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
#endif
static void skip_space(Arq_Lexer *l) {
    while (l->cursor_idx < l->SIZE && (l->at[l->cursor_idx] == 0 || isspace(l->at[l->cursor_idx]))) {
            l->cursor_idx++;
    }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/* cmd_ */
static arq_bool_t is_long_identifier(char chr) {
        return isalnum(chr) || chr == '-' || chr == '_';
}

static arq_bool_t is_short_identifier(char chr) {
        return isalpha(chr) || chr == '?';
}

static arq_bool_t start_short_identifier(Arq_Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && is_short_identifier(l->at[l->cursor_idx + 1])) {
                l->cursor_idx += 2;
                return ARQ_TRUE;
        }
        return ARQ_FALSE;
}

static arq_bool_t start_long_identifier(Arq_Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && l->at[l->cursor_idx + 1] == '-'
        && is_long_identifier(l->at[l->cursor_idx + 2])) {
                l->cursor_idx += 3;
                return ARQ_TRUE;
        }
        return ARQ_FALSE;
}

static arq_bool_t start_dash_dash(Arq_Lexer *l) {
        if (l->at[l->cursor_idx] == '-'
        && l->at[l->cursor_idx + 1] == '-'
        && l->SIZE == 2) {
                l->cursor_idx += 2;
                return ARQ_TRUE;
        }
        return ARQ_FALSE;
}

static Arq_Token next_token(Arq_Lexer *l, arq_bool_t has_identifier) {
        Arq_Token t = {0};
        skip_space(l);
        t.at = &l->at[l->cursor_idx];
        t.size = 0;
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

        if (has_identifier) {
                if (identifier_start(l)) {
                        arq_uint32_t i;
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
                        if (t.at[0] == '0') {
                                return t;
                        }
                        t.size = 0;
                        t.id = ARQ_NO_TOKEN;
                }

        }

        if (l->at[l->cursor_idx] ==  '.') {
                if (l->cursor_idx + 1 < l->SIZE && (isdigit(l->at[l->cursor_idx + 1]))) {
                        dec_float(l, &t);
                        switch (t.id) {
                        case ARQ_DEC_FLOAT: return t;
                        default: break;
                        }; 
                }
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
                if (l->cursor_idx + 1 < l->SIZE && l->at[l->cursor_idx] == '=') {
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
                while (l->cursor_idx < l->SIZE && !isspace(l->at[l->cursor_idx])) {
                        l->cursor_idx++;
                        t.size++;
                }
        }
        return t;
}

void arq_lexer_next_opt_token(Arq_LexerOpt *opt) {
        arq_bool_t has_identifier = ARQ_TRUE;
        opt->lexer.token = next_token(&opt->lexer, has_identifier);
}

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

Arq_LexerOpt arq_lexerOpt_create(void) {
        Arq_LexerOpt opt;
        opt.lexer = arq_lexer_create();
        opt.idx = 0;
        return opt;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static Arq_Token next_cmd_token(Arq_Lexer *lexer) {
        arq_bool_t has_identifier = ARQ_FALSE;
        Arq_Token token = next_token(lexer, has_identifier);
#if 0 
        if (token.id == ARQ_CMD_SHORT_OPTION) {
                return token;
        }
        if (lexer->cursor_idx < lexer->SIZE) {
                token.id = ARQ_CMD_RAW_STR;
                token.size = lexer->SIZE;
                lexer->cursor_idx = lexer->SIZE;
        }
#endif
        return token;
} 

Arq_LexerCmd arq_lexerCmd_create(int argc, char **argv) {
        Arq_LexerCmd cmd;
        cmd.lexer = arq_lexer_create();
        cmd.argc = argc - 1;
        cmd.argv = argv + 1;
        cmd.argIdx = 0;
        cmd.state = 0;
        return cmd;
}

void arq_lexerCmd_reset(Arq_LexerCmd *cmd) {
        cmd->lexer = arq_lexer_create();
        cmd->argIdx = 0;
        return;
}

#if 0
void arq_lexer_next_cmd_token(Arq_LexerCmd *cmd) {
        if (cmd->argIdx >= cmd->argc) {
                cmd->lexer.token.id = ARQ_NO_TOKEN;
                cmd->lexer.SIZE = 0;
                cmd->lexer.cursor_idx = 0;
                cmd->lexer.at = NULL;
                cmd->lexer.token.at = NULL;
                cmd->lexer.token.size = 0;
                cmd->bundeling = ARQ_FALSE;
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
                cmd->bundeling = ARQ_TRUE;
                return;
        }
        if (cmd->argIdx < cmd->argc) {
                cmd->argIdx++;
        }
        cmd->bundeling = ARQ_FALSE;
        return;
}
#else
void arq_lexer_next_cmd_token(Arq_LexerCmd *cmd) {
        if (cmd->argIdx >= cmd->argc) {
                cmd->lexer.token.id = ARQ_NO_TOKEN;
                cmd->lexer.SIZE = 0;
                cmd->lexer.cursor_idx = 0;
                cmd->lexer.at = NULL;
                cmd->lexer.token.at = NULL;
                cmd->lexer.token.size = 0;
                cmd->state = 0; /* init=0, token=1, bundeling=2 */
                return;
        }

        switch (cmd->state) {
        case 0: /* Init */ {
                cmd->lexer.SIZE = strlen(cmd->argv[cmd->argIdx]);
                cmd->lexer.at = cmd->argv[cmd->argIdx];
                cmd->lexer.cursor_idx = 0;
                cmd->lexer.token = next_cmd_token(&cmd->lexer);
                if (cmd->lexer.cursor_idx == cmd->lexer.SIZE) {
                        cmd->argIdx++;
                        return;
                }

                if (cmd->lexer.cursor_idx < cmd->lexer.SIZE &&
                cmd->lexer.token.id == ARQ_CMD_SHORT_OPTION &&
                is_short_identifier(cmd->lexer.at[cmd->lexer.cursor_idx])) {
                        cmd->state = 2; /* bundeling */
                        return;
                }

                if (cmd->lexer.cursor_idx + 1 < cmd->lexer.SIZE &&
                cmd->lexer.at[cmd->lexer.cursor_idx] == '=') {
                        cmd->lexer.cursor_idx++;
                        cmd->state = 1; /* token */
                        return;
                }
                cmd->state = 1; /* token */
                return;
        }
        case 1: /* token */ {
                cmd->lexer.token = next_cmd_token(&cmd->lexer);
                if (cmd->lexer.cursor_idx == cmd->lexer.SIZE) {
                        cmd->state = 0; /* init */
                        cmd->argIdx++;
                        return;
                }

                if (cmd->lexer.cursor_idx + 1 < cmd->lexer.SIZE &&
                cmd->lexer.at[cmd->lexer.cursor_idx] == '=') {
                        cmd->lexer.cursor_idx++;
                        return;
                }
                } return;
        case 2: /* bundeling */ {
                cmd->lexer.token.at = &cmd->lexer.at[cmd->lexer.cursor_idx];
                cmd->lexer.token.id = ARQ_CMD_SHORT_OPTION; 
                cmd->lexer.token.size = 1;
                cmd->lexer.cursor_idx++;

                if (cmd->lexer.cursor_idx == cmd->lexer.SIZE) {
                        cmd->state = 0; /* init */
                        cmd->argIdx++;
                        return;
                }

                if (cmd->lexer.cursor_idx < cmd->lexer.SIZE &&
                is_short_identifier(cmd->lexer.at[cmd->lexer.cursor_idx])) {
                        return;
                }

                if (cmd->lexer.cursor_idx + 1 < cmd->lexer.SIZE &&
                cmd->lexer.at[cmd->lexer.cursor_idx] == '=') {
                        cmd->lexer.cursor_idx++;
                        cmd->state = 1; /* token */
                        return;
                }

                cmd->state = 1; /* token */
                } return;
        default:
                assert(ARQ_FALSE);
                return;
        }
}
#endif

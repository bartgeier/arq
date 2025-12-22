#include "arq_options.h"
#include "arq_symbols.h"
#include "arq_immediate.h"
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

typedef struct {
        uint32_t cursor_idx;
        uint32_t SIZE;
        char const *at;
} Lexer;

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

uint32_t arq_option_parameter_idx(Arq_Option const *option) {
        uint32_t STRLEN;
        uint32_t result = 0;
        if (option->chr != 0) {
                result += 3;
        }
        STRLEN = strlen(option->name);
        if (STRLEN > 0) {
                result += STRLEN + 3;
        }
        return result;
}

#if 0
uint32_to arq_option_verify_vector(Arq_OptVector *tokens, Arq_msg *error_msg) {
        char const *error_str = "' missing open parenthesis '('\n";
        tokens->idx = 0;
        if (arq_imm_L_parenthesis(tokens)) {
                while (tokens->idx < tokens->num_of_token) {
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type(tokens, ARQ_OPT_UINT32_T)) {
                                error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                if (arq_imm_not_identifier(tokens)) {
                                        error_str = "' is not a parameter name\n";
                                        break; /* error */

                                }
                                if (arq_imm_equal(tokens)) {
                                        if (false == arq_imm_is_a_uint32_t(tokens)) {
                                                error_str = "' is not a uint32_t\n";
                                                break; /* error */
                                        }
                                        error_str = "' but expected ',' or ')'\n";
                                } else if (arq_imm_array(tokens)) {
                                        error_str = "' but expected ',' or ')'\n";
                                }
                                if (arq_imm_comma(tokens)) {
                                        continue;
                                }
                                goto next_argument;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type(tokens, ARQ_OPT_INT32_T)) {
                                error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                if (arq_imm_not_identifier(tokens)) {
                                        error_str = "' is not a parameter name\n";
                                        break; /* error */

                                }
                                if (arq_imm_equal(tokens)) {
                                        if (false == arq_imm_is_a_int32_t(tokens)) {
                                                error_str = "' is not a int32_t\n";
                                                break; /* error */
                                        }
                                        error_str = "' but expected ',' or ')'\n";
                                } else if (arq_imm_array(tokens)) {
                                        error_str = "' but expected ',' or ')'\n";
                                }
                                if (arq_imm_comma(tokens)) {
                                        continue;
                                }
                                goto next_argument;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type(tokens, ARQ_OPT_FLOAT)) {
                                error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                if (arq_imm_not_identifier(tokens)) {
                                        error_str = "' is not a parameter name\n";
                                        break; /* error */

                                }
                                if (arq_imm_equal(tokens)) {
                                        if (false == arq_imm_is_a_float(tokens)) {
                                                error_str = "' is not a float\n";
                                                break; /* error */
                                        }
                                        error_str = "' but expected ',' or ')'\n";
                                } else if (arq_imm_array(tokens)) {
                                        error_str = "' but expected ',' or ')'\n";
                                }
                                if (arq_imm_comma(tokens)) {
                                        continue;
                                }
                                goto next_argument;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type(tokens, ARQ_OPT_CSTR_T)) {
                                error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                if (arq_imm_not_identifier(tokens)) {
                                        error_str = "' is not a parameter name\n";
                                        break; /* error */

                                }
                                if (arq_imm_equal(tokens)) {
                                        if (false == arq_imm_is_a_NULL(tokens)) {
                                                error_str =  "' must be NULL\n";
                                                break; /* error */
                                        }
                                        error_str = "' but expected ',' or ')'\n";
                                } else if (arq_imm_array(tokens)) {
                                        error_str = "' but expected ',' or ')'\n";
                                }
                                if (arq_imm_comma(tokens)) {
                                        continue;
                                }
                                goto next_argument;
                        } 
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        error_str = "' is not a type\n";
        next_argument:
                        if (arq_imm_R_parenthesis(tokens)) {
                                error_str = "' after ')' no tokens allowed!\n";
                                if (arq_imm_terminator(tokens)) {
                                        uint32_to idx;
                                        idx.error = false;
                                        idx.u32 = 0;
                                        return idx;
                                } 
                        }
                        break; /* error */
                } /* while */
        }

        {
                /* error */
                Arq_Token token = tokens->at[tokens->idx];
                uint32_to idx; 
                idx.error = true; 
                idx.u32 = tokens->at[tokens->idx].at - tokens->at[0].at;
                arq_msg_clear(error_msg);
                arq_msg_append_cstr(error_msg, "Option failure:\n");
                arq_msg_append_cstr(error_msg, "token '");
                arq_msg_append_str(error_msg, token.at, token.size);
                arq_msg_append_cstr(error_msg, error_str);
                return idx;
        }
}
#endif

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


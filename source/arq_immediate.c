#include "arq_immediate.h"
#include "arq_lexer.h"
#include "arq_token.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/*///////////////////////////////////////////////////////////////////////////*/

void arq_imm_opt_next(Arq_OptVector *opt) {
        if (opt->idx + 1 < opt->num_of_token) {
                opt->idx++;
        }
}

bool arq_imm(Arq_SymbolID const id, Arq_Lexer *opt) {
        const bool b = (opt->token.id == id);
        if (b) {
                arq_lexer_next_opt_token(opt);
        }
        return b;
}

bool arq_imm_noToken(Arq_Token *token) {
        const bool b = (token->id == ARQ_NO_TOKEN);
        return b;
}

bool arq_imm_not_identifier(Arq_Lexer *opt) {
        const bool b = (opt->token.id == ARQ_IDENTFIER);
        if (b) {
                arq_lexer_next_opt_token(opt);
        }
        return !b;
}

bool arq_imm_literal_uint(Arq_Lexer *opt) {
        uint32_to num;
        switch (opt->token.id) {
        case ARQ_P_DEC:
                num = arq_tok_pDec_to_uint32_t(&opt->token, NULL, "");
                break;
        case ARQ_HEX:
                num = arq_tok_hex_to_uint32_t(&opt->token, NULL, "");
                break;
        default:
                return false;
        }
        if (!num.error) {
                /* success */
                arq_lexer_next_opt_token(opt);
        }
        return !num.error; /* return true if successful */
}

bool arq_imm_literal_int(Arq_Lexer *opt) {
        int32_to num;
        switch (opt->token.id) {
        case ARQ_P_DEC: case ARQ_N_DEC:
                num = arq_tok_sDec_to_int32_t(&opt->token, NULL, "");
                break;
        case ARQ_HEX: {
                uint32_to const n = arq_tok_hex_to_uint32_t(&opt->token, NULL, "");
                num.i32 = (int32_t)n.u32;
                num.error = n.error;
                } break;
        default:
                return false;
        }
        if (!num.error) {
                /* success */
                arq_lexer_next_opt_token(opt);
        }
        return !num.error; /* return true if successful */
}

bool arq_imm_literal_float(Arq_Lexer *opt) {
        float_to num;
        switch (opt->token.id) {
        case ARQ_DEC_FLOAT:
                num = arq_tok_decFloat_to_float(&opt->token);
                break;
        case ARQ_HEX_FLOAT:
                num = arq_tok_hexFloat_to_float(&opt->token);
                break;
        default:
                return false;
        }
        if (!num.error) {
                /* success */
                arq_lexer_next_opt_token(opt);
        }
        return !num.error; /* return true if successful */
}

uint32_to arq_imm_default_uint32_t(Arq_Lexer *opt) {
        uint32_to num = {0};
        switch (opt->token.id) {
        case ARQ_P_DEC: 
                num = arq_tok_pDec_to_uint32_t(&opt->token, NULL, "");
                break;
        case ARQ_HEX:
                num = arq_tok_hex_to_uint32_t(&opt->token, NULL, "");
                break;
        default:
                assert(false);
                break;
        }
        assert(num.error == false);
        arq_lexer_next_opt_token(opt);
        return num;
}

int32_to arq_imm_default_int32_t(Arq_Lexer *opt) {
        int32_to num = {0};
        switch (opt->token.id) {
        case ARQ_P_DEC: case ARQ_N_DEC:
                num = arq_tok_sDec_to_int32_t(&opt->token, NULL, "");
                break;
        case ARQ_HEX: {
                uint32_to const x = arq_tok_hex_to_uint32_t(&opt->token, NULL, "");
                num.i32 = (int32_t)x.u32;
                num.error = x.error;
                } break;
        default:
                assert(false);
                break;
        }
        assert(num.error == false);
        arq_lexer_next_opt_token(opt);
        return num;
}

float_to arq_imm_default_float(Arq_Lexer *opt) {
        float_to num = {0};
        switch (opt->token.id) {
        case ARQ_DEC_FLOAT:
                num = arq_tok_decFloat_to_float(&opt->token);
                break;
        case ARQ_HEX_FLOAT:
                num = arq_tok_hexFloat_to_float(&opt->token);
                break;
        default:
                assert(false);
                break;
        }
        assert(num.error == false);
        arq_lexer_next_opt_token(opt);
        return num;
}

bool arq_imm_is_a_NULL(Arq_Lexer *opt) {
        if (opt->token.id != ARQ_NULL) {
                return false;
        }
        arq_lexer_next_opt_token(opt);
        return true;
}

char const *arq_imm_default_cstr_t(Arq_Lexer *opt) {
        arq_lexer_next_opt_token(opt);
        return NULL;
}

/*///////////////////////////////////////////////////////////////////////////*/

bool arq_imm_cmd_is_dashdash(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_CMD_DASHDASH);
        if (b) {
                arq_imm_cmd_next(cmd);
        }
        return b;
}

/* jumps over a bundel of short options */
/* -shello */
/* s is a short option */
/* if s take no argument then 'h' and maybe 'ello' are all short options */
/* if s takes an argument then 'hello' is the argument */
/*     'next_bundle_idx' over jumps 'hello' because in the vector they all bundled short options */
/*     'hello' should be a cstring in the vector but it isn't. */
/*     That's why we have to increment idx for every char (short option) in the bundle 'hello' */
static void next_bundle_idx(Arq_Vector *v) {
        char const *const begin = v->at[v->idx].at;
        char const *const end = begin + strlen(begin);
        char const *x = begin;
        assert(*end == 0);
        while ((v->idx < v->num_of_token - 1) && (x >= begin) && (x < end)) {
                v->idx++;
                x = v->at[v->idx].at;
        }
}

void arq_imm_cmd_next(Arq_Vector *cmd) {
        if (cmd->idx + 1 < cmd->num_of_token) {
                cmd->idx++;
        }
}

bool arq_imm_cmd_has_token_left(Arq_Vector *cmd) {
        return cmd->idx < cmd->num_of_token;
}

bool arq_imm_cmd_is_long_option(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_CMD_LONG_OPTION);
        return b;
}

bool arq_imm_cmd_is_short_option(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_CMD_SHORT_OPTION);
        return b;
}

bool arq_imm_is_p_dec(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_P_DEC);
        return b;
}

bool arq_imm_is_n_dec(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_N_DEC);
        return b;
}

bool arq_imm_is_hex(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_HEX);
        return b;
}

bool arq_imm_is_hexFloat(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_DEC_FLOAT) || (token->id == ARQ_HEX_FLOAT);
        return b;
}

Arq_Lexer arq_imm_get_long(
        Arq_Option const *options,
        uint32_t const num_of_options,
        uint32_t *idx,
        Arq_Vector *cmd,
        Arq_msg *error_msg
) {
        Arq_Lexer lexer = arq_lexer_create();
        Arq_Token const *token = &cmd->at[cmd->idx];
        for (*idx = 0; *idx < num_of_options; (*idx)++) {
                if (token_long_option_eq(token, options[*idx].name)) {
                        lexer.at = options[*idx].arguments;
                        lexer.SIZE = strlen(options[*idx].arguments);
                        lexer.cursor_idx = 0;
                        arq_imm_cmd_next(cmd);
                        return lexer;
                }
        }
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        /* arq_msg_append_cstr(error_msg, "'--"); */
        arq_msg_append_str(error_msg, token->at, token->size);
        arq_msg_append_cstr(error_msg, "' unknown long option ");
        arq_msg_append_lf(error_msg);
        return lexer;
}

Arq_Lexer arq_imm_get_short(
        Arq_Option const *options,
        uint32_t const num_of_options,
        uint32_t *idx,
        Arq_Vector *cmd,
        Arq_msg *error_msg
) {
        Arq_Token const token = cmd->at[cmd->idx];
        uint32_t const IDX = (token.at[0] == '-') ? 1 : 0; /* : 0 because of bundled short options */
        Arq_Lexer lexer = arq_lexer_create();
        for (*idx = 0; *idx < num_of_options; (*idx)++) {
                if (token.at[IDX] == options[*idx].chr) {
                        lexer.at = options[*idx].arguments;
                        lexer.SIZE = strlen(options[*idx].arguments);
                        lexer.cursor_idx = 0;
                        arq_imm_cmd_next(cmd);
                        return lexer;
                }
        }
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        /* arq_msg_append_cstr(error_msg, "'-"); */
        arq_msg_append_str(error_msg, token.at, token.size);
        arq_msg_append_cstr(error_msg, "' unknown short option ");
        arq_msg_append_lf(error_msg);
        return lexer; 
}

void arq_imm_cmd_not_a_option(Arq_Vector const *cmd, Arq_msg *error_msg) {
        Arq_Token const token = cmd->at[cmd->idx];
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        arq_msg_append_cstr(error_msg, "'");
        arq_msg_append_str(error_msg, token.at, token.size);
        arq_msg_append_cstr(error_msg, "' is not an option");
        arq_msg_append_lf(error_msg);
}

bool arq_imm_end_of_line(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_NO_TOKEN);
        return b;
}

bool arq_imm_optional_argument_uint32_t(Arq_Vector *cmd, uint32_to *num, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        switch (token->id) {
        case ARQ_P_DEC:
                *num = arq_tok_pDec_to_uint32_t(token, error_msg, CMD_LINE_FAILURE);
                break;
        case ARQ_HEX:
                *num = arq_tok_hex_to_uint32_t(token, error_msg, CMD_LINE_FAILURE);
                break;
        default:
                return false;
        }
        if (num->error) {
                return true; /* overflow */
        } 
        arq_imm_cmd_next(cmd);
        return false;
}

bool arq_imm_optional_argument_int32_t(Arq_Vector *cmd, int32_to *num, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        switch (token->id) {
        case ARQ_P_DEC:
        case ARQ_N_DEC:
                *num = arq_tok_sDec_to_int32_t(token, error_msg, CMD_LINE_FAILURE);
                break;
        case ARQ_HEX: {
                uint32_to n = arq_tok_hex_to_uint32_t(token, error_msg, CMD_LINE_FAILURE);
                num->i32 = (int32_t)n.u32;
                num->error = n.error;
                } break;
        default:
                return false;
        }
        if (num->error) {
                return true; /* overflow */
        } 
        arq_imm_cmd_next(cmd);
        return false;
}

bool arq_imm_optional_argument_float(Arq_Vector *cmd, float_to *num, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        (void)error_msg;
        switch (token->id) {
        case ARQ_DEC_FLOAT:
                *num = arq_tok_decFloat_to_float(token);
                break;
        case ARQ_HEX_FLOAT:
                *num = arq_tok_hexFloat_to_float(token);
                break;
        default:
                return false;
        }
        if (num->error) {
                return true;
        }
        arq_imm_cmd_next(cmd);
        return false;
}

bool arq_imm_optional_argument_cstr_t(Arq_Vector *cmd, char const **cstr) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        if (token->id != ARQ_CMD_LONG_OPTION 
        && token->id != ARQ_CMD_SHORT_OPTION) {
                *cstr = token->at;
                if (*cstr != NULL) {
                        next_bundle_idx(cmd);
                        return true;
                }
        }
        return false;
}

bool arq_imm_pick_cstr_t(Arq_Vector *cmd, char const **cstr) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        if (token->id != ARQ_NO_TOKEN) {
                *cstr = arq_imm_argument_csrt_t(cmd, NULL);
                return true;
        }
        *cstr = NULL;
        return false;
}

uint32_to arq_imm_argument_uint32_t(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        uint32_to result = {0};
        char const *cstr = CMD_LINE_FAILURE;
        switch (token->id) {
        case ARQ_HEX:
                result = arq_tok_hex_to_uint32_t(token, error_msg, cstr);
                break;
        case ARQ_P_DEC:
                result = arq_tok_pDec_to_uint32_t(token, error_msg, cstr);
                break;
        default:
                if (error_msg != NULL) {
                        Arq_Token const tok = *token;
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' is not a positiv number");
                        arq_msg_append_lf(error_msg);
                }
                result.error = true;
                return result;
        }
        arq_imm_cmd_next(cmd);
        return result;
}

int32_to arq_imm_argument_int32_t(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        int32_to result = {0};
        char const *cstr = CMD_LINE_FAILURE;
        switch (token->id) {
        case ARQ_HEX: {
                uint32_to const r = arq_tok_hex_to_uint32_t(token, error_msg, cstr);
                result.i32 = (int32_t) r.u32;
                result.error = r.error;
                } break;
        case ARQ_P_DEC:
        case ARQ_N_DEC:
                result = arq_tok_sDec_to_int32_t(token, error_msg, cstr);
                break;
        default:
                if (error_msg != NULL) {
                        Arq_Token const tok = *token;
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' is not a signed number");
                        arq_msg_append_lf(error_msg);
                }
                result.error = true;
                return result;
        }
        arq_imm_cmd_next(cmd);
        return result;
}

float_to arq_imm_argument_float(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        float_to result = {0};
        char const *cstr = CMD_LINE_FAILURE;
        switch (token->id) {
        case ARQ_HEX_FLOAT:
                result = arq_tok_hexFloat_to_float(token);
                break;
        case ARQ_DEC_FLOAT:
                result = arq_tok_decFloat_to_float(token);
                break;
        default:
                if (error_msg != NULL) {
                        Arq_Token const tok = *token;
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' is not a float number");
                        arq_msg_append_lf(error_msg);
                }
                result.error = true;
                return result;
        }
        arq_imm_cmd_next(cmd);
        return result;
}

char const *arq_imm_argument_csrt_t(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token *token = &cmd->at[cmd->idx];
        char const *result;
        if (token->id == ARQ_NO_TOKEN) {
                Arq_Token const tok = *token;
                arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
                arq_msg_append_cstr(error_msg, "Token '");
                arq_msg_append_str(error_msg, tok.at, tok.size);
                arq_msg_append_cstr(error_msg, "' is not a c string => expected an argument");
                arq_msg_append_lf(error_msg);
                result = NULL;
                return result;
        } 

        /* Even it looks like a short or long option but it is not it expects an argument */
        result = token->at;
        next_bundle_idx(cmd);
        return result;
}


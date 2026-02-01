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

bool arq_imm(Arq_SymbolID const id, Arq_LexerOpt *opt) {
        const bool b = (opt->lexer.token.id == id);
        if (b) {
                arq_lexer_next_opt_token(opt);
        }
        return b;
}

bool arq_imm_noToken(Arq_Token *token) {
        const bool b = (token->id == ARQ_NO_TOKEN);
        return b;
}

bool arq_imm_not_identifier(Arq_LexerOpt *opt) {
        const bool b = (opt->lexer.token.id == ARQ_IDENTFIER);
        if (b) {
                arq_lexer_next_opt_token(opt);
        }
        return !b;
}

bool arq_imm_literal_uint_error(Arq_LexerOpt *opt,  Arq_msg *error_msg) {
        uint_o num;
        switch (opt->lexer.token.id) {
        case ARQ_P_DEC:
                num = arq_tok_pDec_to_uint(&opt->lexer.token, error_msg, OPTION_FAILURE);
                break;
        case ARQ_HEX:
                num = arq_tok_hex_to_uint(&opt->lexer.token, error_msg, OPTION_FAILURE);
                break;
        default:
                arq_msg_clear(error_msg);
                arq_msg_append_cstr(error_msg, OPTION_FAILURE);
                arq_msg_append_str(error_msg, opt->lexer.token.at, opt->lexer.token.size);
                arq_msg_append_cstr(error_msg, "' is not a uint literal\n");
                num.u = 0;
                num.error = true;
                break;
        }
        if (!num.error) {
                /* success */
                arq_lexer_next_opt_token(opt);
        }
        return num.error; /* return true if successful */
}

bool arq_imm_literal_int_error(Arq_LexerOpt *opt,  Arq_msg *error_msg) {
        int_o num;
        switch (opt->lexer.token.id) {
        case ARQ_P_DEC: case ARQ_N_DEC:
                num = arq_tok_sDec_to_int(&opt->lexer.token, error_msg, OPTION_FAILURE);
                break;
        case ARQ_HEX: {
                uint_o const n = arq_tok_hex_to_uint(&opt->lexer.token, NULL, "");
                num.i = (int32_t)n.u;
                num.error = n.error;
                } break;
        default:
                arq_msg_clear(error_msg);
                arq_msg_append_cstr(error_msg, OPTION_FAILURE);
                arq_msg_append_str(error_msg, opt->lexer.token.at, opt->lexer.token.size);
                arq_msg_append_cstr(error_msg, "' is not a int literal\n");
                num.i = 0;
                num.error = true;
                break;
        }
        if (!num.error) {
                /* success */
                arq_lexer_next_opt_token(opt);
        }
        return num.error; /* return true if successful */
}

bool arq_imm_literal_float_error(Arq_LexerOpt *opt,  Arq_msg *error_msg) {
        float_o num;
        switch (opt->lexer.token.id) {
        case ARQ_DEC_FLOAT:
                num = arq_tok_decFloat_to_float(&opt->lexer.token);
                break;
        case ARQ_HEX_FLOAT:
                num = arq_tok_hexFloat_to_float(&opt->lexer.token);
                break;
        default:
                arq_msg_clear(error_msg);
                arq_msg_append_cstr(error_msg, OPTION_FAILURE);
                arq_msg_append_str(error_msg, opt->lexer.token.at, opt->lexer.token.size);
                arq_msg_append_cstr(error_msg, "' is not a float literal\n");
                num.f = 0.0;
                num.error = true;
                break;
        }
        if (!num.error) {
                /* success */
                arq_lexer_next_opt_token(opt);
        }
        return num.error; /* return true if successful */
}

bool arq_imm_literal_NULL_error(Arq_LexerOpt *opt,  Arq_msg *error_msg) {
        bool const b = opt->lexer.token.id == ARQ_NULL;
        if (b) {
                /* success */
                arq_lexer_next_opt_token(opt);
                return false;
        } else {
                arq_msg_clear(error_msg);
                arq_msg_append_cstr(error_msg, OPTION_FAILURE);
                arq_msg_append_str(error_msg, opt->lexer.token.at, opt->lexer.token.size);
                arq_msg_append_cstr(error_msg, "' must be NULL\n");
                return true;
        }
}

union_o arq_imm_default_uint(Arq_LexerOpt *opt) {
        union_o num = {0};
        switch (opt->lexer.token.id) {
        case ARQ_P_DEC: 
                num.ou = arq_tok_pDec_to_uint(&opt->lexer.token, NULL, "");
                break;
        case ARQ_HEX:
                num.ou = arq_tok_hex_to_uint(&opt->lexer.token, NULL, "");
                break;
        default:
                assert(false);
                break;
        }
        assert(num.ou.error == false);
        arq_lexer_next_opt_token(opt);
        return num;
}

union_o arq_imm_default_int(Arq_LexerOpt *opt) {
        union_o num = {0};
        switch (opt->lexer.token.id) {
        case ARQ_P_DEC: case ARQ_N_DEC:
                num.oi = arq_tok_sDec_to_int(&opt->lexer.token, NULL, "");
                break;
        case ARQ_HEX: {
                uint_o const x = arq_tok_hex_to_uint(&opt->lexer.token, NULL, "");
                num.oi.i = (int32_t)x.u;
                num.oi.error = x.error;
                } break;
        default:
                assert(false);
                break;
        }
        assert(num.oi.error == false);
        arq_lexer_next_opt_token(opt);
        return num;
}

union_o arq_imm_default_float(Arq_LexerOpt *opt) {
        union_o num = {0};
        switch (opt->lexer.token.id) {
        case ARQ_DEC_FLOAT:
                num.of = arq_tok_decFloat_to_float(&opt->lexer.token);
                break;
        case ARQ_HEX_FLOAT:
                num.of = arq_tok_hexFloat_to_float(&opt->lexer.token);
                break;
        default:
                assert(false);
                break;
        }
        assert(num.of.error == false);
        arq_lexer_next_opt_token(opt);
        return num;
}

char const *arq_imm_default_cstr_t(Arq_LexerOpt *opt) {
        arq_lexer_next_opt_token(opt);
        return NULL;
}

union_o arq_imm_default_value(Arq_LexerOpt *opt) {
        union_o a = {0};
        (void) opt;
        return a;
}

/*///////////////////////////////////////////////////////////////////////////*/

bool arq_imm_cmd_is_dashdash(Arq_LexerCmd *cmd) {
        Arq_Token const *token = &cmd->lexer.token;
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
static void next_bundle_idx(Arq_LexerCmd *cmd) {
        char const *const begin = cmd->lexer.token.at;
        char const *const end = begin + strlen(begin);
        char const *x = begin;
        assert(*end == 0);
        while ((cmd->lexer.token.id != ARQ_NO_TOKEN) && (x >= begin) && (x < end)) {
                arq_lexer_next_cmd_token(cmd);
                x = cmd->lexer.token.at;
        }
}

void arq_imm_cmd_next(Arq_LexerCmd *cmd) {
        if (cmd->lexer.token.id != ARQ_NO_TOKEN) {
                arq_lexer_next_cmd_token(cmd);
        }
}

bool arq_imm_cmd_has_token_left(Arq_LexerCmd *cmd) {
        return cmd->lexer.token.id != ARQ_NO_TOKEN;
}

bool arq_imm_cmd_is_long_option(Arq_LexerCmd *cmd) {
        return (cmd->lexer.token.id == ARQ_CMD_LONG_OPTION);
}

bool arq_imm_cmd_is_short_option(Arq_LexerCmd *cmd) {
        return (cmd->lexer.token.id == ARQ_CMD_SHORT_OPTION);
}


bool arq_imm_is_uint(Arq_LexerCmd *cmd) {
        return (cmd->lexer.token.id == ARQ_P_DEC) || (cmd->lexer.token.id == ARQ_HEX);
}

bool arq_imm_is_int(Arq_LexerCmd *cmd) {
        return (cmd->lexer.token.id == ARQ_P_DEC) || (cmd->lexer.token.id == ARQ_N_DEC) || (cmd->lexer.token.id == ARQ_HEX);
}

bool arq_imm_is_float(Arq_LexerCmd *cmd) {
        return (cmd->lexer.token.id == ARQ_DEC_FLOAT) || (cmd->lexer.token.id == ARQ_HEX_FLOAT);
}

Arq_LexerOpt arq_imm_get_long(
        Arq_Option const *options,
        uint32_t const num_of_options,
        Arq_LexerCmd *cmd,
        Arq_msg *error_msg
) {
        Arq_LexerOpt opt = arq_lexerOpt_create();
        Arq_Token const *token = &cmd->lexer.token;
        for (opt.idx = 0; opt.idx < num_of_options; opt.idx++) {
                if (token_long_option_eq(token, options[opt.idx].name)) {
                        opt.lexer.at = options[opt.idx].arguments;
                        opt.lexer.SIZE = strlen(options[opt.idx].arguments);
                        opt.lexer.cursor_idx = 0;
                        arq_imm_cmd_next(cmd);
                        return opt;
                }
        }
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        /* arq_msg_append_cstr(error_msg, "'--"); */
        arq_msg_append_str(error_msg, token->at, token->size);
        arq_msg_append_cstr(error_msg, "' unknown long option ");
        arq_msg_append_lf(error_msg);
        return opt;
}
Arq_LexerOpt arq_imm_get_short(
        Arq_Option const *options,
        uint32_t const num_of_options,
        Arq_LexerCmd *cmd,
        Arq_msg *error_msg
) {
        Arq_Token const *token = &cmd->lexer.token;
        uint32_t const IDX = (token->at[0] == '-') ? 1 : 0; /* : 0 because of bundled short options */
        Arq_LexerOpt opt = arq_lexerOpt_create();
        for (opt.idx = 0; opt.idx < num_of_options; opt.idx++) {
                if (token->at[IDX] == options[opt.idx].chr) {
                        opt.lexer.at = options[opt.idx].arguments;
                        opt.lexer.SIZE = strlen(options[opt.idx].arguments);
                        opt.lexer.cursor_idx = 0;
                        arq_imm_cmd_next(cmd);
                        return opt;
                }
        }
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        /* arq_msg_append_cstr(error_msg, "'-"); */
        arq_msg_append_str(error_msg, token->at, token->size);
        arq_msg_append_cstr(error_msg, "' unknown short option ");
        arq_msg_append_lf(error_msg);
        return opt; 
}

void arq_imm_cmd_not_a_option(Arq_LexerCmd const *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->lexer.token;
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        arq_msg_append_str(error_msg, token->at, token->size);
        arq_msg_append_cstr(error_msg, "' is not an option");
        arq_msg_append_lf(error_msg);
}
bool arq_imm_end_of_line(Arq_LexerCmd *cmd) {
        return (cmd->lexer.token.id == ARQ_NO_TOKEN);
}

bool arq_imm_optional_argument_uint(Arq_LexerCmd *cmd, union_o *num, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->lexer.token;
        switch (token->id) {
        case ARQ_P_DEC:
                num->ou = arq_tok_pDec_to_uint(token, error_msg, CMD_LINE_FAILURE);
                break;
        case ARQ_HEX:
                num->ou = arq_tok_hex_to_uint(token, error_msg, CMD_LINE_FAILURE);
                break;
        default:
                return false;
        }
        if (num->ou.error) {
                return true; /* overflow */
        } 
        arq_imm_cmd_next(cmd);
        return false;
}

bool arq_imm_optional_argument_int(Arq_LexerCmd *cmd, union_o *num, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->lexer.token;
        switch (token->id) {
        case ARQ_P_DEC:
        case ARQ_N_DEC:
                num->oi = arq_tok_sDec_to_int(token, error_msg, CMD_LINE_FAILURE);
                break;
        case ARQ_HEX: {
                uint_o n = arq_tok_hex_to_uint(token, error_msg, CMD_LINE_FAILURE);
                num->oi.i = (int32_t)n.u;
                num->oi.error = n.error;
                } break;
        default:
                return false;
        }
        if (num->oi.error) {
                return true; /* overflow */
        } 
        arq_imm_cmd_next(cmd);
        return false;
}

bool arq_imm_optional_argument_float(Arq_LexerCmd *cmd, union_o *num, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->lexer.token;
        (void)error_msg;
        switch (token->id) {
        case ARQ_DEC_FLOAT:
                num->of = arq_tok_decFloat_to_float(token);
                break;
        case ARQ_HEX_FLOAT:
                num->of = arq_tok_hexFloat_to_float(token);
                break;
        default:
                return false;
        }
        if (num->of.error) {
                return true;
        }
        arq_imm_cmd_next(cmd);
        return false;
}

bool arq_imm_optional_argument_cstr_t(Arq_LexerCmd *cmd, char const **cstr) {
        Arq_Token const *token = &cmd->lexer.token;
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

bool arq_imm_pick_cstr_t(Arq_LexerCmd *cmd, char const **cstr) {
        Arq_Token const *token = &cmd->lexer.token;
        if (token->id != ARQ_NO_TOKEN) {
                *cstr = arq_imm_argument_csrt_t(cmd, NULL);
                return true;
        }
        *cstr = NULL;
        return false;
}

union_o arq_imm_argument_uint(Arq_LexerCmd *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->lexer.token;
        union_o result = {0};
        char const *cstr = CMD_LINE_FAILURE;
        switch (token->id) {
        case ARQ_HEX:
                result.ou = arq_tok_hex_to_uint(token, error_msg, cstr);
                break;
        case ARQ_P_DEC:
                result.ou = arq_tok_pDec_to_uint(token, error_msg, cstr);
                break;
        default:
                if (error_msg != NULL) {
                        Arq_Token const tok = *token;
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' is not a positiv number");
                        arq_msg_append_lf(error_msg);
                }
                result.ou.error = true;
                return result;
        }
        arq_imm_cmd_next(cmd);
        return result;
}

union_o arq_imm_argument_int(Arq_LexerCmd *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->lexer.token;
        union_o result = {0};
        char const *cstr = CMD_LINE_FAILURE;
        switch (token->id) {
        case ARQ_HEX: {
                uint_o const r = arq_tok_hex_to_uint(token, error_msg, cstr);
                result.oi.i = (int32_t) r.u;
                result.oi.error = r.error;
                } break;
        case ARQ_P_DEC:
        case ARQ_N_DEC:
                result.oi = arq_tok_sDec_to_int(token, error_msg, cstr);
                break;
        default:
                if (error_msg != NULL) {
                        Arq_Token const tok = *token;
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' is not a signed number");
                        arq_msg_append_lf(error_msg);
                }
                result.oi.error = true;
                return result;
        }
        arq_imm_cmd_next(cmd);
        return result;
}


union_o arq_imm_argument_float(Arq_LexerCmd *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->lexer.token;
        union_o result = {0};
        char const *cstr = CMD_LINE_FAILURE;
        switch (token->id) {
        case ARQ_HEX_FLOAT:
                result.of = arq_tok_hexFloat_to_float(token);
                break;
        case ARQ_DEC_FLOAT:
                result.of = arq_tok_decFloat_to_float(token);
                break;
        default:
                if (error_msg != NULL) {
                        Arq_Token const tok = *token;
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' is not a float number");
                        arq_msg_append_lf(error_msg);
                }
                result.of.error = true;
                return result;
        }
        arq_imm_cmd_next(cmd);
        return result;
}

char const *arq_imm_argument_csrt_t(Arq_LexerCmd *cmd, Arq_msg *error_msg) {
        Arq_Token *token = &cmd->lexer.token;
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


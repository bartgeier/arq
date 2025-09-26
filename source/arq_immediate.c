#include "arq_immediate.h"
#include "arq_symbols.h"
#include "arq_conversion.h"
#include "arq_token.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

void arq_imm_opt_next(Arq_OptVector *opt) {
        if (opt->idx + 1 < opt->num_of_token) {
                opt->idx++;
        }
}

bool arq_imm_type(Arq_OptVector *opt, Arq_SymbolID const id) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == id);
        if (b) {
                arq_imm_opt_next(opt);
        }
        return b;
}

bool arq_imm_equal(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == ARQ_OPT_EQ);
        if (b) {
                arq_imm_opt_next(opt);
        }
        return b;
}

bool arq_imm_array(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == ARQ_OPT_ARRAY);
        if (b) {
                arq_imm_opt_next(opt);
        }
        return b;
}

bool arq_imm_comma(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == ARQ_OPT_COMMA);
        if (b) {
                arq_imm_opt_next(opt);
        }
        return b;
}

bool arq_imm_L_parenthesis(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == ARQ_OPT_L_PARENTHESIS);
        if (b) {
                arq_imm_opt_next(opt);
        }
        return b;
}

bool arq_imm_R_parenthesis(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == ARQ_OPT_R_PARENTHESIS);
        if (b) {
                arq_imm_opt_next(opt);
        }
        return b;
}

bool arq_imm_terminator(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == ARQ_OPT_TERMINATOR);
        return b;
}

bool arq_imm_not_identifier(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        const bool b = (token->id == ARQ_OPT_IDENTFIER);
        if (b) {
                arq_imm_opt_next(opt);
        }
        return !b;
}

bool arq_imm_is_a_uint32_t(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        if (token->id != ARQ_P_NUMBER) {
                return false;
        }
        uint32_to const num = arq_tok_pNumber_to_uint32_t(token, NULL, "");
        bool const success = !num.error;
        if (success) {
                arq_imm_opt_next(opt);
        }
        return success;
}

uint32_to arq_imm_default_uint32_t(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        uint32_to num = arq_tok_pNumber_to_uint32_t(token, NULL, "");
        assert(num.error == false);
        arq_imm_opt_next(opt);
        return num;
}

bool arq_imm_is_a_NULL(Arq_OptVector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        if (token->id != ARQ_OPT_NULL) {
                return false;
        }
        arq_imm_opt_next(opt);
        return true;
}

char const *arq_imm_default_cstr_t(Arq_OptVector *opt) {
        arq_imm_opt_next(opt);
        return NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool arq_imm_cmd_is_dashdash(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_CMD_DASHDASH);
        if (b) {
                arq_imm_cmd_next(cmd);
                // arq_imm_opt_next(opt);
        }
        return b;
}

// jumps over a bundel of short options
// -shello
// s is a short option
// if s take no argument then 'h' and maybe 'ello' are all short options
// if s takes an argument then 'hello' is the argument
//     'next_bundle_idx' over jumps 'hello' because in the vector they all bundled short options
//     'hello' should be a cstring in the vector but it isn't.
//     That's why we have to increment idx for every char (short option) in the bundle 'hello'
static void next_bundle_idx(Arq_Vector *v) {
        char const *const begin = v->at[v->idx].at;
        char const *const end = begin + strlen(begin);
        assert(*end == 0);
        char const *x = begin;
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
        if (b) {
                // arq_imm_cmd_next(cmd);
        }
        return b;
}

bool arq_imm_cmd_is_short_option(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_CMD_SHORT_OPTION);
        if (b) {
                // arq_imm_cmd_next(cmd);
        }
        return b;
}

bool arq_imm_is_p_number(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_P_NUMBER);
        return b;
}

bool arq_imm_is_raw_str(Arq_Vector *cmd) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        const bool b = (token->id == ARQ_P_NUMBER);
        return b;
}

Arq_OptVector *arq_imm_get_long(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector *cmd ,
        Arq_msg *error_msg
) {
        for (uint32_t i = 0; i < option_list->num_of_Vec; i++) {
                Arq_Token const *cmd_token = &cmd->at[cmd->idx];
                char const * opt_name = options[i].name;
                if (string_eq(cmd_token, opt_name)) {
                        option_list->row = i;
                        Arq_OptVector *opt = option_list->at[option_list->row];
                        opt->idx = 0;
                        arq_imm_cmd_next(cmd);
                        return opt;
                }
        }
        Arq_Token const token = cmd->at[cmd->idx];
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        arq_msg_append_cstr(error_msg, "'--");
        arq_msg_append_str(error_msg, token.at, token.size);
        arq_msg_append_cstr(error_msg, "' unknown long option ");
        arq_msg_append_lf(error_msg);
        return NULL;
}

Arq_OptVector *arq_imm_get_short(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector *cmd,
        Arq_msg *error_msg
) {
        //assert(strlen(cmd->at[cmd->idx].at) == 1);
        for (uint32_t i = 0; i < option_list->num_of_Vec; i++) {
                char cmd_token = cmd->at[cmd->idx].at[0];
                char opt_short_name = options[i].chr;
                if (cmd_token == opt_short_name) {
                        option_list->row = i;
                        Arq_OptVector *opt = option_list->at[option_list->row];
                        opt->idx = 0;
                        arq_imm_cmd_next(cmd);
                        return opt;
                }
        }
        Arq_Token const token = cmd->at[cmd->idx];
        arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
        arq_msg_append_cstr(error_msg, "'-");
        arq_msg_append_str(error_msg, token.at, token.size);
        arq_msg_append_cstr(error_msg, "' unknown short option ");
        arq_msg_append_lf(error_msg);
        return NULL; 
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
        const bool b = (token->id == ARQ_CMD_END_OF_LINE);
        return b;
}

bool arq_imm_optional_argument_uint32_t(Arq_Vector *cmd, uint32_to *num, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        if (token->id != ARQ_P_NUMBER) {
                return false;
        }
        *num = arq_tok_pNumber_to_uint32_t(token, error_msg, CMD_LINE_FAILURE);
        if (num->error) {
                return true; // overflow
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
        if (token->id != ARQ_CMD_END_OF_LINE) {
                *cstr = arq_imm_argument_csrt_t(cmd, NULL);
                //if (*cstr != NULL) {
                        //next_bundle_idx(cmd);
                        return true;
                //}
        }
        *cstr = NULL;
        return false;
}
uint32_to arq_imm_argument_uint32_t(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        uint32_to result = {0};
        char const *cstr = CMD_LINE_FAILURE;
        if (token->id != ARQ_P_NUMBER) {
                if (error_msg != NULL) {
                        Arq_Token const tok = *token;
                        // arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' is not a positiv number");
                        arq_msg_append_lf(error_msg);
                }
                result.error = true;
                return result;
        }
        result = arq_tok_pNumber_to_uint32_t(token, error_msg, cstr);
        arq_imm_cmd_next(cmd);
        return result;
}

char const *arq_imm_argument_csrt_t(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token *token = &cmd->at[cmd->idx];
        char const *result;
        if (token->id == ARQ_CMD_END_OF_LINE) {
                Arq_Token const tok = *token;
                // arq_msg_clear(error_msg);
                arq_msg_append_cstr(error_msg, CMD_LINE_FAILURE);
                arq_msg_append_cstr(error_msg, "Token '");
                arq_msg_append_str(error_msg, tok.at, tok.size);
                arq_msg_append_cstr(error_msg, "' is not a c string => expected an argument");
                arq_msg_append_lf(error_msg);
                result = NULL;
                return result;
        } 

        // Even it looks like a short or long option but it is not it expects an argument
        char const chr = token->at[-1];
        if (token->id == ARQ_CMD_SHORT_OPTION) {
                if (chr == '-') {
                        token->at -= 1;    // -foo
                        token->size += 1;
                }
        } else if (cmd->at[cmd->idx].id == ARQ_CMD_LONG_OPTION) {
             token->at -= 2;   // --foo
             token->size += 2;
        }
        result = token->at;
        next_bundle_idx(cmd);
        return result;
}


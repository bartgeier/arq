#include "arq_immediate.h"
#include "arq_symbols.h"
#include "arq_conversion.h"
#include "arq_token.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

bool arq_imm_cmd_has_token_left(Arq_Vector *cmd) {
        return cmd->idx < cmd->num_of_token;
}

bool arq_imm_cmd_is_long_option(Arq_Vector *cmd) {
        const bool b = (cmd->at[cmd->idx].id == ARQ_CMD_LONG_OPTION);
        return b;
        //v->idx += b ? 1 : 0;
}

bool arq_imm_cmd_is_short_option(Arq_Vector *cmd) {
        const bool b = (cmd->at[cmd->idx].id == ARQ_CMD_SHORT_OPTION);
        return b;
        //v->idx += b ? 1 : 0;
}

Arq_Vector *arq_imm_get_long(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector const *cmd
) {
        for (uint32_t i = 0; i < option_list->num_of_Vec; i++) {
                Arq_Token const *cmd_token = &cmd->at[cmd->idx];
                char const * opt_name = options[i].name;
                if (string_eq(cmd_token, opt_name)) {
                        option_list->row = i;
                        Arq_Vector *opt = option_list->at[option_list->row];
                        opt->idx = 0;
                        return opt;
                }
        }
        return NULL;
}

Arq_Vector *arq_imm_get_short(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector const *cmd
) {
        //assert(strlen(cmd->at[cmd->idx].at) == 1);
        for (uint32_t i = 0; i < option_list->num_of_Vec; i++) {
                char cmd_token = cmd->at[cmd->idx].at[0];
                char opt_short_name = options[i].chr;
                if (cmd_token == opt_short_name) {
                        option_list->row = i;
                        Arq_Vector *opt = option_list->at[option_list->row];
                        opt->idx = 0;
                        return opt;
                }
        }
        return NULL;
}

void arq_imm_next(Arq_Vector *v) {
        if (v->idx + 1 < v->num_of_token) {
                v->idx++;
        }
}

bool arq_imm_type(Arq_Vector *opt, Arq_SymbolID const id) {
        const bool b = (opt->at[opt->idx].id == id);
        opt->idx += b ? 1 : 0;
        return b;
}

bool arq_imm_assignment_equal(Arq_Vector *opt) {
        const bool b = (opt->at[opt->idx].id == ARQ_PARA_EQ);
        //opt->idx += b ? 1 : 0;
        arq_imm_next(opt);
        return b;
}

uint32_to arq_imm_default_uint32_t(Arq_Vector *opt) {
        Arq_Token const *token = &opt->at[opt->idx];
        uint32_to num = arq_tok_pNumber_to_uint32_t(token, NULL, "");
        assert(num.error == false);
        arq_imm_next(opt);
        return num;
}

char const *arq_imm_default_cstr_t(Arq_Vector *opt) {
        arq_imm_next(opt);
        return NULL;
}

// return true in case of an error in converting the number
bool arq_imm_if_uint32_t(Arq_Vector *cmd, uint32_to *num, Arq_msg *error_msg) {
        if (cmd->at[cmd->idx].id != ARQ_P_NUMBER) {
                return false;
        }
        *num = arq_tok_pNumber_to_uint32_t(&cmd->at[cmd->idx], error_msg, "CMD line failure:\n");
        if (num->error == false) {
                arq_imm_next(cmd);
                return true;
        } else {
                return false;
        }
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

void arq_imm_if_cstr_t(Arq_Vector *cmd, char const **cstr) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        if (token->id != ARQ_CMD_LONG_OPTION 
        && token->id != ARQ_CMD_SHORT_OPTION) {
                *cstr = token->at;
                if (*cstr != NULL) {
                        next_bundle_idx(cmd);
                }
        }
}

uint32_to arq_imm_take_uint32_t(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token const *token = &cmd->at[cmd->idx];
        uint32_to result = {0};
        char const *cstr = "CMD line failure:\n";
        if (token->id != ARQ_P_NUMBER) {
                if (error_msg != NULL) {
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, token->at, token->size);
                        arq_msg_append_cstr(error_msg, "' is not a positiv number");
                        arq_msg_append_lf(error_msg);
                }
                result.error = true;
                return result;
        }
        result = arq_tok_pNumber_to_uint32_t(token, error_msg, cstr);
        arq_imm_next(cmd);
        return result;
}

char const *arq_imm_take_csrt_t(Arq_Vector *cmd, Arq_msg *error_msg) {
        Arq_Token *token = &cmd->at[cmd->idx];
        char const *result;
        if (cmd->at[cmd->idx].id == ARQ_END) {
                arq_msg_append_cstr(error_msg, "CMD line failure:\n");
                arq_msg_append_cstr(error_msg, "Token '");
                arq_msg_append_str(error_msg, cmd->at[cmd->idx].at, cmd->at[cmd->idx].size);
                arq_msg_append_cstr(error_msg, "' is not a c string");
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


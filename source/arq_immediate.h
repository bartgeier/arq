#ifndef ARQ_IMMEDIATE_H
#define ARQ_IMMEDIATE_H

#include "arq_conversion.h"
#include "arq_lexer.h"
#include "arq_bool.h"

#define CMD_LINE_FAILURE "CMD line failure:\n"

/*///////////////////////////////////////////////////////////////////////////*/

void arq_imm_opt_next(Arq_OptVector *opt);
bool arq_imm_type(Arq_Lexer *opt, Arq_SymbolID const id);

bool arq_imm_equal(Arq_Lexer *opt);
bool arq_imm_array(Arq_Lexer *opt);
bool arq_imm_comma(Arq_Lexer *opt);
bool arq_imm_L_parenthesis(Arq_Lexer *opt);
bool arq_imm_R_parenthesis(Arq_Lexer *opt);
bool arq_imm_terminator(Arq_OptVector *opt);
bool arq_imm_noToken(Arq_Token *token);

bool arq_imm_not_identifier(Arq_Lexer *opt);

bool arq_imm_is_a_uint32_t(Arq_Lexer *opt);
bool arq_imm_is_a_int32_t(Arq_Lexer *opt);
bool arq_imm_is_a_float(Arq_Lexer *opt);

uint32_to arq_imm_default_uint32_t(Arq_Lexer *opt);
int32_to arq_imm_default_int32_t(Arq_Lexer *opt);
float_to arq_imm_default_float(Arq_Lexer *opt);

bool arq_imm_is_a_NULL(Arq_Lexer *opt);
char const *arq_imm_default_cstr_t(Arq_Lexer *opt);

/*///////////////////////////////////////////////////////////////////////////*/

bool arq_imm_cmd_is_dashdash(Arq_Vector *cmd);

void arq_imm_cmd_next(Arq_Vector *cmd);
bool arq_imm_cmd_has_token_left(Arq_Vector *cmd);
bool arq_imm_end_of_line(Arq_Vector *cmd);

Arq_Lexer arq_imm_get_long(
        Arq_Option const *options,
        uint32_t const num_of_options,
        uint32_t *idx,
        Arq_Vector *cmd,
        Arq_msg *error_msg
);
Arq_Lexer arq_imm_get_short(
        Arq_Option const *options,
        uint32_t const num_of_options,
        uint32_t *idx,
        Arq_Vector *cmd,
        Arq_msg *error_msg
);
void arq_imm_cmd_not_a_option(Arq_Vector const *cmd, Arq_msg *error_msg);

bool arq_imm_cmd_is_long_option(Arq_Vector *cmd);
bool arq_imm_cmd_is_short_option(Arq_Vector *cmd);

bool arq_imm_is_p_dec(Arq_Vector *cmd);
bool arq_imm_is_n_dec(Arq_Vector *cmd);
bool arq_imm_is_hex(Arq_Vector *cmd);
bool arq_imm_is_hexFloat(Arq_Vector *cmd);

bool arq_imm_optional_argument_uint32_t(Arq_Vector *cmd, uint32_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_int32_t(Arq_Vector *cmd, int32_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_cstr_t(Arq_Vector *cmd, char const **cstr);
bool arq_imm_optional_argument_float(Arq_Vector *cmd, float_to *num, Arq_msg *error_msg);
bool arq_imm_pick_cstr_t(Arq_Vector *cmd, char const **cstr);

uint32_to arq_imm_argument_uint32_t(Arq_Vector *cmd, Arq_msg *error_msg);
int32_to arq_imm_argument_int32_t(Arq_Vector *cmd, Arq_msg *error_msg);
float_to arq_imm_argument_float(Arq_Vector *cmd, Arq_msg *error_msg);
char const *arq_imm_argument_csrt_t(Arq_Vector *cmd, Arq_msg *error_msg);

#endif 

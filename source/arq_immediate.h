#ifndef ARQ_IMMEDIATE_H
#define ARQ_IMMEDIATE_H

#include "arq.h"
#include "arq_symbols.h"
#include "arq_token.h"
#include "arq_conversion.h"
#include "arq_bool.h"

#define CMD_LINE_FAILURE "CMD line failure:\n"

/*///////////////////////////////////////////////////////////////////////////*/

void arq_imm_opt_next(Arq_OptVector *opt);
bool arq_imm_type(Arq_OptVector *opt, Arq_SymbolID const id);

bool arq_imm_equal(Arq_OptVector *opt);
bool arq_imm_array(Arq_OptVector *opt);
bool arq_imm_comma(Arq_OptVector *opt);
bool arq_imm_L_parenthesis(Arq_OptVector *opt);
bool arq_imm_R_parenthesis(Arq_OptVector *opt);
bool arq_imm_terminator(Arq_OptVector *opt);

bool arq_imm_not_identifier(Arq_OptVector *opt);

bool arq_imm_is_a_uint8_t(Arq_OptVector *opt); 
bool arq_imm_is_a_uint16_t(Arq_OptVector *opt);
bool arq_imm_is_a_uint32_t(Arq_OptVector *opt); 
bool arq_imm_is_a_int8_t(Arq_OptVector *opt);
bool arq_imm_is_a_int16_t(Arq_OptVector *opt);
bool arq_imm_is_a_int32_t(Arq_OptVector *opt);

uint8_to arq_imm_default_uint8_t(Arq_OptVector *opt);
uint16_to arq_imm_default_uint16_t(Arq_OptVector *opt);
uint32_to arq_imm_default_uint32_t(Arq_OptVector *opt);
int8_to arq_imm_default_int8_t(Arq_OptVector *opt);
int16_to arq_imm_default_int16_t(Arq_OptVector *opt);
int32_to arq_imm_default_int32_t(Arq_OptVector *opt);

bool arq_imm_is_a_NULL(Arq_OptVector *opt);
char const *arq_imm_default_cstr_t(Arq_OptVector *opt);

/*///////////////////////////////////////////////////////////////////////////*/

bool arq_imm_cmd_is_dashdash(Arq_Vector *cmd);

void arq_imm_cmd_next(Arq_Vector *cmd);
bool arq_imm_cmd_has_token_left(Arq_Vector *cmd);
bool arq_imm_end_of_line(Arq_Vector *cmd);

Arq_OptVector *arq_imm_get_long(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector *cmd,
        Arq_msg *error_msg
);
Arq_OptVector *arq_imm_get_short(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector *cmd,
        Arq_msg *error_msg
);
void arq_imm_cmd_not_a_option(Arq_Vector const *cmd, Arq_msg *error_msg);

bool arq_imm_cmd_is_long_option(Arq_Vector *cmd);
bool arq_imm_cmd_is_short_option(Arq_Vector *cmd);

bool arq_imm_is_p_dec(Arq_Vector *cmd);
bool arq_imm_is_n_dec(Arq_Vector *cmd);
bool arq_imm_is_hex(Arq_Vector *cmd);

bool arq_imm_optional_argument_uint8_t(Arq_Vector *cmd, uint8_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_uint16_t(Arq_Vector *cmd, uint16_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_uint32_t(Arq_Vector *cmd, uint32_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_int8_t(Arq_Vector *cmd, int8_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_int16_t(Arq_Vector *cmd, int16_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_int32_t(Arq_Vector *cmd, int32_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_cstr_t(Arq_Vector *cmd, char const **cstr);
bool arq_imm_pick_cstr_t(Arq_Vector *cmd, char const **cstr);

uint8_to arq_imm_argument_uint8_t(Arq_Vector *cmd, Arq_msg *error_msg);
uint16_to arq_imm_argument_uint16_t(Arq_Vector *cmd, Arq_msg *error_msg);
uint32_to arq_imm_argument_uint32_t(Arq_Vector *cmd, Arq_msg *error_msg);
int8_to arq_imm_argument_int8_t(Arq_Vector *cmd, Arq_msg *error_msg);
int16_to arq_imm_argument_int16_t(Arq_Vector *cmd, Arq_msg *error_msg);
int32_to arq_imm_argument_int32_t(Arq_Vector *cmd, Arq_msg *error_msg);
char const *arq_imm_argument_csrt_t(Arq_Vector *cmd, Arq_msg *error_msg);

#endif 

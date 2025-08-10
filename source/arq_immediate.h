#ifndef ARQ_IMMEDIATE_H
#define ARQ_IMMEDIATE_H

#include "arq.h"
#include "arq_symbols.h"
#include "arq_token.h"
#include "arq_conversion.h"
#include <stdbool.h>


///////////////////////////////////////////////////////////////////////////////

void arq_imm_opt_next(Arq_OptVector *opt);
bool arq_imm_type(Arq_OptVector *opt, Arq_SymbolID const id);

bool arq_imm_equal(Arq_OptVector *opt);
bool arq_imm_comma(Arq_OptVector *opt);
bool arq_imm_terminator(Arq_OptVector *opt);

uint32_to arq_imm_default_uint32_t(Arq_OptVector *opt);
char const *arq_imm_default_cstr_t(Arq_OptVector *opt);

///////////////////////////////////////////////////////////////////////////////

void arq_imm_cmd_next(Arq_Vector *cmd);
bool arq_imm_cmd_has_token_left(Arq_Vector *cmd);
bool arq_imm_end_of_line(Arq_Vector *cmd);

Arq_OptVector *arq_imm_get_long(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector *cmd
);
Arq_OptVector *arq_imm_get_short(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector *cmd
);

bool arq_imm_cmd_is_long_option(Arq_Vector *cmd);
bool arq_imm_cmd_is_short_option(Arq_Vector *cmd);

bool arq_imm_optional_argument_uint32_t(Arq_Vector *cmd, uint32_to *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_cstr_t(Arq_Vector *cmd, char const **cstr);

uint32_to arq_imm_argument_uint32_t(Arq_Vector *cmd, Arq_msg *error_msg);
char const *arq_imm_argument_csrt_t(Arq_Vector *cmd, Arq_msg *error_msg);

#endif 

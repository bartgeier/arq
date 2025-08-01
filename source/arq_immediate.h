#ifndef ARQ_IMMEDIATE_H
#define ARQ_IMMEDIATE_H

#include "arq.h"
#include "arq_symbols.h"
#include "arq_token.h"
#include "arq_conversion.h"
#include <stdbool.h>

bool arq_imm_cmd_has_token_left(Arq_Vector *cmd);
bool arq_imm_cmd_is_long_option(Arq_Vector *cmd);
bool arq_imm_cmd_is_short_option(Arq_Vector *cmd);

Arq_Vector *arq_imm_get_long(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector const *cmd
);
Arq_Vector *arq_imm_get_short(
        Arq_List *option_list,
        Arq_Option const *options,
        Arq_Vector const *cmd
);

void arq_imm_next(Arq_Vector *v);

bool arq_imm_type(Arq_Vector *opt, Arq_SymbolID const id);
bool arq_imm_assignment_equal(Arq_Vector *opt);
uint32_to arq_imm_default_uint32_t(Arq_Vector *opt);
char const *arq_imm_default_cstr_t(Arq_Vector *opt);

bool arq_imm_if_uint32_t(Arq_Vector *cmd, uint32_to *num, Arq_msg *error_msg);
void arq_imm_if_cstr_t(Arq_Vector *cmd, char const **cstr);
uint32_to arq_imm_take_uint32_t(Arq_Vector *cmd, Arq_msg *error_msg);
char const *arq_imm_take_csrt_t(Arq_Vector *cmd, Arq_msg *error_msg);

#endif 

#ifndef ARQ_IMMEDIATE_H
#define ARQ_IMMEDIATE_H

#include "arq_conversion.h"
#include "arq_lexer.h"
#include "arq.h"
#include "arq_bool.h"

#define CMD_LINE_FAILURE "CMD line failure:\nToken '"
#define OPTION_FAILURE "Option failure:\nToken '"

typedef  bool (*arq_fn_imm_literal_error)(Arq_LexerOpt*,  Arq_msg*);


/*///////////////////////////////////////////////////////////////////////////*/

void arq_imm_opt_next(Arq_OptVector *opt);

bool arq_imm(Arq_SymbolID const id, Arq_LexerOpt *opt);
bool arq_imm_noToken(Arq_Token *token);
bool arq_imm_not_identifier(Arq_LexerOpt *opt);

bool arq_imm_literal_uint_error(Arq_LexerOpt *opt,  Arq_msg *error_msg);
bool arq_imm_literal_int_error(Arq_LexerOpt *opt,  Arq_msg *error_msg);
bool arq_imm_literal_float_error(Arq_LexerOpt *opt,  Arq_msg *error_msg);
bool arq_imm_literal_NULL_error(Arq_LexerOpt *opt,  Arq_msg *error_msg);

typedef union_o (*arq_imm_default)(Arq_LexerOpt *opt);
union_o arq_imm_default_uint(Arq_LexerOpt *opt);
union_o arq_imm_default_int(Arq_LexerOpt *opt);
union_o arq_imm_default_float(Arq_LexerOpt *opt);
char const *arq_imm_default_cstr_t(Arq_LexerOpt *opt);

bool arq_imm_is_a_NULL(Arq_LexerOpt *opt);

/*///////////////////////////////////////////////////////////////////////////*/

bool arq_imm_cmd_is_dashdash(Arq_LexerCmd *cmd);

void arq_imm_cmd_next(Arq_LexerCmd *cmd);
bool arq_imm_cmd_has_token_left(Arq_LexerCmd *cmd);
bool arq_imm_end_of_line(Arq_LexerCmd *cmd);

Arq_LexerOpt arq_imm_get_long(
        Arq_Option const *options,
        uint32_t const num_of_options,
        Arq_LexerCmd *cmd,
        Arq_msg *error_msg
);
Arq_LexerOpt arq_imm_get_short(
        Arq_Option const *options,
        uint32_t const num_of_options,
        Arq_LexerCmd *cmd,
        Arq_msg *error_msg
);

void arq_imm_cmd_not_a_option(Arq_LexerCmd const *cmd, Arq_msg *error_msg);
bool arq_imm_cmd_is_long_option(Arq_LexerCmd *cmd);
bool arq_imm_cmd_is_short_option(Arq_LexerCmd *cmd);

typedef bool (*arq_imm_is)(Arq_LexerCmd *cmd);
bool arq_imm_is_uint(Arq_LexerCmd *cmd);
bool arq_imm_is_int(Arq_LexerCmd *cmd);
bool arq_imm_is_float(Arq_LexerCmd *cmd);

typedef bool (*arq_imm_optional_argument)(Arq_LexerCmd *cmd, union_o *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_uint(Arq_LexerCmd *cmd, union_o *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_int(Arq_LexerCmd *cmd, union_o *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_float(Arq_LexerCmd *cmd, union_o *num, Arq_msg *error_msg);
bool arq_imm_optional_argument_cstr_t(Arq_LexerCmd *cmd, char const **cstr);
bool arq_imm_pick_cstr_t(Arq_LexerCmd *cmd, char const **cstr);

typedef union_o (*arq_imm_argument)(Arq_LexerCmd *cmd, Arq_msg *error_msg);
union_o arq_imm_argument_uint(Arq_LexerCmd *cmd, Arq_msg *error_msg);
union_o arq_imm_argument_int(Arq_LexerCmd *cmd, Arq_msg *error_msg);
union_o arq_imm_argument_float(Arq_LexerCmd *cmd, Arq_msg *error_msg);
char const *arq_imm_argument_csrt_t(Arq_LexerCmd *cmd, Arq_msg *error_msg);

#endif 

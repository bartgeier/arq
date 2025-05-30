#ifndef ARQ_OPTIONS_H
#define ARQ_OPTIONS_H

#include "arq.h"
#include "arq_token.h"
#include "arq_string.h"

uint32_t arq_num_of_option_token(Arq_Option const *option);
uint32_t arq_option_parameter_idx(Arq_Option const *option);
void arq_tokenize_option(Arq_Option const *option, Arq_Vector *v, uint32_t num_of_token);
uint32_to arg_verify_vector(Arq_Vector const *tokens, Error_msg *error_msg);

#endif

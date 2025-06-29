#ifndef ARQ_OPTIONS_H
#define ARQ_OPTIONS_H

#include "arq.h"
#include "arq_token.h"
#include "arq_tok.h"

uint32_t arq_option_parameter_idx(Arq_Option const *option);
void arq_option_tokenize(Arq_Option const *option, Arq_Vector *v, uint32_t const num_of_token);
uint32_to arq_option_verify_vector(Arq_Vector const *tokens, Arq_msg *error_msg);

#endif

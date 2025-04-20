#ifndef ARQ_OPTIONS_H
#define ARQ_OPTIONS_H

#include "arq.h"
#include "arq_token.h"

uint32_t arq_num_of_option_token(Arq_Option const *option);
void arq_tokenize_option(Arq_Option const *option, Arq_Vector *v, uint32_t num_of_token);

#endif

#ifndef ARQ_CMD_H
#define ARQ_CMD_H

#include <stdint.h>
#include "arq_token.h"

uint32_t arq_cmd_num_of_token(int argc, char **argv);
void arq_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t num_of_token);

#endif


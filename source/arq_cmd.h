#ifndef ARQ_CMD_H
#define ARQ_CMD_H

#include <stdint.h>
#include "arq_token.h"

uint32_t arq_num_of_cmd_token(int argc, char **argv);
void arq_tokenize_cmd(int argc, char **argv, Arq_Vector *v, uint32_t num_of_token);


#endif

#ifndef ARQ_CMD_H
#define ARQ_CMD_H

#include <stdint.h>
#include "arq_token.h"

#ifdef __cplusplus
extern "C" {
#endif

void arq_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t num_of_token);

#ifdef __cplusplus
}
#endif
#endif


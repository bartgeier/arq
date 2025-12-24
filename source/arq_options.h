#ifndef ARQ_OPTIONS_H
#define ARQ_OPTIONS_H

#include "arq.h"
#include "arq_token.h"

typedef struct {
        uint32_t cursor_idx;
        uint32_t SIZE;
        char const *at;
} Lexer;

#ifdef __cplusplus
extern "C" {
#endif

Arq_Token arq_next_opt_token(Lexer *l);
void arq_option_tokenize(Arq_Option const *option, Arq_OptVector *v, uint32_t const num_of_token);
void arq_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t const num_of_token);

#ifdef __cplusplus
}
#endif
#endif

#ifndef ARQ_LEXER_H
#define ARQ_LEXER_H

#include "arq_token.h"
#include "arq_bool.h"

typedef struct {
        uint32_t cursor_idx;
        uint32_t SIZE;
        char const *at;
        Arq_Token token;
} Arq_Lexer;

typedef struct {
        Arq_Lexer lexer;
        uint32_t idx;
} Arq_LexerOpt;

typedef struct {
        Arq_Lexer lexer;
        bool bundeling;
        int argc;
        char **argv;
        int argIdx;
} Arq_LexerCmd;

#ifdef __cplusplus
extern "C" {
#endif

Arq_Lexer arq_lexer_create(void);
Arq_LexerOpt arq_lexerOpt_create(void);
Arq_LexerCmd arq_lexerCmd_create(int argc, char **argv);

void arq_lexer_next_opt_token(Arq_LexerOpt *l);


void arq_lexerCmd_reset(Arq_LexerCmd *cmd);
void arq_lexer_next_cmd_token(Arq_LexerCmd *l);
void arq_lexer_cmd_tokenize(int argc, char **argv, Arq_Vector *v, uint32_t const num_of_token);

#ifdef __cplusplus
}
#endif
#endif

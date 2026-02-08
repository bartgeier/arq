#include "arq_log.h"

#ifdef ARQ_LOG_TOKENS
static void log_print_token_member(Arq_Token *t, uint32_t toknr) {
        uint32_t i;
        printf("%3d %30s -> ", toknr, symbol_names[t->id]);
        printf("%2d ", t->size);
        for (i = 0; i < t->size; i++) {
                putchar(t->at[i]);
        }
        printf("\n");
}

void log_options_tokens(Arq_Option const *options, uint32_t const num_of_options) {
        uint32_t n;
        for (n = 0; n < num_of_options; n++) {
                uint32_t i = 0;
                Arq_LexerOpt opt = arq_lexerOpt_create();
                opt.lexer.at = options[n].arguments;
                opt.lexer.SIZE = strlen(options[n].arguments);
                opt.lexer.cursor_idx = 0;
                printf("Option[%d] -%c --%s %s\n", n, options[n].chr, options[n].name, options[n].arguments);
                do {
                        arq_lexer_next_opt_token(&opt);
                        log_print_token_member(&opt.lexer.token, i++);
                } while (opt.lexer.token.id != ARQ_NO_TOKEN);
                printf("\n");
        }
}

void log_cmd_tokens(int argc, char **argv)  {
        Arq_LexerCmd cmd = arq_lexerCmd_create(argc, argv);
        uint32_t i = 0;
        printf("Command line tokens:\n");
        do {
                arq_lexer_next_cmd_token(&cmd);
                log_print_token_member(&cmd.lexer.token, i++);

        } while(cmd.lexer.token.id != ARQ_NO_TOKEN);
        printf("\n");
}
#endif

#ifdef ARQ_LOG_TOKENIZER
void log_int_uint(uint_o const *x) {
        printf("    ");
        printf("%d\n", x->u);
}
void log_int_int(int_o const *x) {
        printf("    ");
        printf("%d\n", x->i);
}
void log_int_float(float_o const *x) {
        printf("    ");
        printf("%f\n", x->f);
}
#endif

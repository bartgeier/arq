#ifndef ARQ_LOG_H
#define ARQ_LOG_H

#ifdef ARQ_LOG_MEMORY
        #include <stdio.h>
        #define log_memory(args) do {   \
                        printf args;    \
                        printf("\n");   \
                } while (0)
                /* do { printf(fmt "\n", ##__VA_ARGS__); } while (0) */
#else
        #define log_memory(args) do {} while (0)
#endif

#ifdef ARQ_LOG_TOKENS
        #include "arq_token.h"
        #include "arq_int.h"
        #include "arq_lexer.h"
        #include "arq.h"
        #include <string.h>
        #include <stdio.h>
        static void log_print_token_member(Arq_Token *t, uint32_t toknr) {
                uint32_t i;
                printf("%3d %30s -> ", toknr, symbol_names[t->id]);
                printf("%2d ", t->size);
                for (i = 0; i < t->size; i++) {
                        putchar(t->at[i]);
                }
                printf("\n");
        }

        static void log_options_tokens(Arq_Option const *options, uint32_t const num_of_options) {
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

        static void log_cmd_tokens(int argc, char **argv)  {
                Arq_LexerCmd cmd = arq_lexerCmd_create(argc, argv);
                uint32_t i = 0;
                printf("Command line tokens:\n");
                do {
                        arq_lexer_next_cmd_token(&cmd);
                        log_print_token_member(&cmd.lexer.token, i++);

                } while(cmd.lexer.token.id != ARQ_NO_TOKEN);
                printf("\n");
        }
#else
        #define log_print_token_member(token, nr) ((void)0)
        #define log_options_tokens(opt, num_of_options) ((void)0)
        #define log_cmd_tokens(argc, argv) ((void)0)
#endif

#ifdef ARQ_LOG_TOKENIZER
        /* used for interpreter logging */
        #include <stdio.h>
        #define log_int_banner(fmt) \
                do { printf("---------" fmt "------------\n"); } while (0)
        #define log_int_ln() \
                do { printf("\n"); } while (0)
        #define log_int_comment(fmt) \
                do { printf("    " fmt "\n"); } while (0)
        #define log_int_token(fmt) \
                do { printf("%s\n",symbol_names[fmt]); } while (0)
        #define log_int_token_indent(fmt) \
                do { printf("    %s\n",symbol_names[fmt]); } while (0)
        #define log_inta(args) do {     \
                        printf("    "); \
                        printf args;    \
                        printf("\n");   \
                } while (0)
#else
        #define log_int_banner(fmt) do {} while (0)
        #define log_int_ln() do {} while (0)
        #define log_int_comment(fmt) do {} while (0)
        #define log_int_token(fmt) do {} while (0)
        #define log_int_token_indent(fmt) do {} while (0)
        #define log_inta(args) do {} while (0)
#endif

#endif

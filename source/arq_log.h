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

#ifdef ARQ_LOG_TOKENIZER
        #include "arq_token.h"
        #include "arq_int.h"
        #include "arq_lexer.h"
        #include <string.h>
        #include <stdio.h>
        static void log_tokenizer(Arq_Token *t, uint32_t toknr) {
                uint32_t i;
                printf("%3d %30s -> ", toknr, symbol_names[t->id]);
                printf("%2d ", t->size);
                for (i = 0; i < t->size; i++) {
                        putchar(t->at[i]);
                }
                printf("\n");
        }

        static void log_tokenizer_option_t(Arq_Option const *options, uint32_t const num_of_options) {
                uint32_t n;
                for (n = 0; n < num_of_options; n++) {
                        uint32_t i = 0;
                        Arq_Lexer l = arq_lexer_create();
                        l.at = options[n].arguments;
                        l.SIZE = strlen(options[n].arguments);
                        l.cursor_idx = 0;
                        printf("Option[%d] -%c --%s %s\n", n, options[n].chr, options[n].name, options[n].arguments);
                        do {
                                arq_lexer_next_opt_token(&l);
                                log_tokenizer(&l.token, i++);
                        } while (l.token.id != ARQ_OPT_NO_TOKEN);
                        printf("\n");
                }
        }

        static void log_tokenizer_cmd_line(Arq_Vector *v)  {
                uint32_t i;
                printf("Command line token:\n");
                for (i = 0; i < v->num_of_token; i++) {
                        log_tokenizer(&v->at[i], i);
                }
                printf("\n");
        }
#else
        #define log_tokenizer(t) ((void)0)
        #define log_tokenizer_option(v, number) ((void)0)
        #define log_tokenizer_option_t(o, n) ((void)0)
        #define log_tokenizer_cmd_line(v) ((void)0)
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

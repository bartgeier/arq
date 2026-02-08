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
        void log_print_token_member(Arq_Token *t, uint32_t toknr);
        void log_options_tokens(Arq_Option const *options, uint32_t const num_of_options);
        void log_cmd_tokens(int argc, char **argv);
#else
        #define log_print_token_member(token, nr) ((void)0)
        #define log_options_tokens(opt, num_of_options) ((void)0)
        #define log_cmd_tokens(argc, argv) ((void)0)
#endif

#ifdef ARQ_LOG_TOKENIZER
        /* used for interpreter logging */
        #include "arq_conversion.h"
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
        #define log_inta(args) do { \
                printf("    ");     \
                printf args ;       \
                printf("\n");       \
        } while (0)
        void log_int_uint(uint_o const *x);
        void log_int_int(int_o const *x);
        void log_int_float(float_o const *x);
#else
        #define log_int_banner(fmt) do {} while (0)
        #define log_int_ln() do {} while (0)
        #define log_int_comment(fmt) do {} while (0)
        #define log_int_token(fmt) do {} while (0)
        #define log_int_token_indent(fmt) do {} while (0)
        #define log_inta(args) do {} while (0)
        #define log_int_uint(x) ((void)0);
        #define log_int_int(x) ((void)0);
        #define log_int_float(x) ((void)0);
#endif

#endif

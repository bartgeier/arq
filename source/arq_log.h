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
        #include <stdio.h>
        static inline void log_tokenizer(Arq_Token *t, uint32_t toknr) {
                printf("%3d %30s -> ", toknr, symbol_names[t->id]);
                printf("%2d ", t->size);
                for (uint32_t i = 0; i < t->size; i++) {
                        putchar(t->at[i]);
                }
                printf("\n");
        }

        static inline void log_tokenizer_option(Arq_OptVector *v, uint32_t number) {
                printf("Option token %d:\n", number);
                for (uint32_t j = 0; j < v->num_of_token; j++) {
                        log_tokenizer(&v->at[j], j);
                }
                printf("\n");
        }

        static inline void log_tokenizer_cmd_line(Arq_Vector *v)  {
                printf("Command line token:\n");
                for (uint32_t j = 0; j < v->num_of_token; j++) {
                        log_tokenizer(&v->at[j], j);
                }
                printf("\n");
        }
#else
        #define log_tokenizer(t) ((void)0)
        #define log_tokenizer_option(v, number) ((void)0)
        #define log_tokenizer_cmd_line(v) ((void)0)
#endif

#ifdef ARQ_LOG_TOKENIZER
        // used for interpreter logging
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

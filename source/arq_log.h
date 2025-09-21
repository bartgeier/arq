#ifndef ARQ_LOG_H
#define ARQ_LOG_H


#ifdef ARQ_LOG_TOKENIZER
        #include <stdio.h>
        static inline void log_arq_token(Arq_Token *t, uint32_t toknr) {
                printf("%3d %30s -> ", toknr, symbol_names[t->id]);
                printf("%2d ", t->size);
                for (uint32_t i = 0; i < t->size; i++) {
                        putchar(t->at[i]);
                }
                printf("\n");
        }

        static inline void log_arq_option_token(Arq_OptVector *v, uint32_t number) {
                printf("Option token %d:\n", number);
                for (uint32_t j = 0; j < v->num_of_token; j++) {
                        log_arq_token(&v->at[j], j);
                }
                printf("\n");
        }

        static inline void log_arq_cmd_line_token(Arq_Vector *v)  {
                printf("Command line token:\n");
                for (uint32_t j = 0; j < v->num_of_token; j++) {
                        log_arq_token(&v->at[j], j);
                }
                printf("\n");
        }
#else
        #define log_arq_token(t) ((void)0)
        #define log_arq_option_token(v, number) ((void)0)
        #define log_arq_cmd_line_token(v) ((void)0)
#endif

#ifdef ARQ_LOG_MEMORY
        #include <stdio.h>
        #define log_arq_mem(fmt, ...) \
                do { printf(fmt "\n", ##__VA_ARGS__); } while (0)
#else
        #define log_arq_mem(fmt, ...) do {} while (0)
#endif

#ifdef ARQ_LOG_INFO
        #include <stdio.h>
        #define log_arq(fmt, ...) \
                do { fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__); } while (0)
        #define log_arq_banner(fmt) \
                do { printf("---------" fmt "------------\n"); } while (0)
#else
        #define log_arq(fmt, ...) do {} while (0)
        #define log_arq_banner(fmt) do {} while (0)
#endif

#endif

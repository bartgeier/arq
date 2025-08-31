#ifndef ARQ_SYMBOLS_H
#define ARQ_SYMBOLS_H

#define ARQ_LIST_OF_IDS                          \
                                                 \
        /* helper to control option tokenizer */ \
        X(ARQ_OPT_NO_TOKEN)                      \
        X(ARQ_OPT_IDENTFIER)                     \
                                                 \
        /* literals */                           \
        X(ARQ_P_NUMBER)                          \
        X(ARQ_N_NUMBER)                          \
        X(ARQ_CMD_RAW_STR)                       \
                                                 \
        /* option operators */                   \
        X(ARQ_OPT_EQ)                            \
        X(ARQ_OPT_COMMA)                         \
        X(ARQ_OPT_TERMINATOR)                    \
        X(ARQ_OPT_UNKNOWN)                       \
                                                 \
        /* option keywords */                    \
        X(ARQ_OPT_NULL)                          \
        /* option types */                       \
        X(ARQ_OPT_CSTR_T)                        \
        X(ARQ_OPT_UINT8_T)                       \
        X(ARQ_OPT_UINT16_T)                      \
        X(ARQ_OPT_UINT32_T)                      \
        X(ARQ_OPT_UINT64_T)                      \
        X(ARQ_OPT_INT8_T)                        \
        X(ARQ_OPT_INT16_T)                       \
        X(ARQ_OPT_INT32_T)                       \
        X(ARQ_OPT_INT64_T)                       \
                                                 \
        /* command line tokens */                \
        X(ARQ_CMD_SHORT_OPTION)                  \
        X(ARQ_CMD_LONG_OPTION)                   \
        X(ARQ_CMD_DASHDASH)                      \
        X(ARQ_CMD_END_OF_LINE)                   \

#define X(name)name,
typedef enum {
        ARQ_LIST_OF_IDS
} Arq_SymbolID;
#undef X

extern char *symbol_names[];

#endif

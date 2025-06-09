#ifndef ARQ_SYMBOLS_H
#define ARQ_SYMBOLS_H

#define ARQ_LIST_OF_IDS   \
        X(ARQ_INIT) \
\
        X(ARQ_PARA_SPACE_TAIL) \
        X(ARQ_PARA_N_PARAMETER) \
        X(ARQ_PARA_O_PARAMETER) \
        X(ARQ_PARA_IDENTFIER)  \
        X(ARQ_PARA_NULL)   \
        X(ARQ_P_NUMBER)   \
        X(ARQ_N_NUMBER)   \
        X(ARQ_PARA_EQ)         \
        X(ARQ_PARA_COMMA)      \
        X(ARQ_END)        \
        X(ARQ_PARA_UNKNOWN)    \
\
        X(ARQ_PARA_CSTR_T)     \
        X(ARQ_PARA_UINT8_T)    \
        X(ARQ_PARA_UINT16_T)   \
        X(ARQ_PARA_UINT32_T)   \
        X(ARQ_PARA_UINT64_T)   \
        X(ARQ_PARA_INT8_T)     \
        X(ARQ_PARA_INT16_T)    \
        X(ARQ_PARA_INT32_T)    \
        X(ARQ_PARA_INT64_T) \
\
        X(ARQ_CMD_EMPTY) \
        X(ARQ_CMD_SHORT_OPTION) \
        X(ARQ_CMD_LONG_OPTION) \
        X(ARQ_CMD_OPTION_TERMINATOR) \
        X(ARQ_CMD_RAW_STR) \
        X(ARQ_CMD_ERROR_UNKNOWN) \
        X(ARQ_CMD_ERROR_NOT_IDENTIFIER) \
        X(ARQ_CMD_ERROR_NOT_NUMBER) \

#if 0
X(ARQ_CMD_P_NUMBER) 
X(ARQ_CMD_N_NUMBER) 
#endif

#define X(name)name,
typedef enum {
        ARQ_LIST_OF_IDS
} Arq_TypeID;
#undef X

extern char *symbol_names[];

#endif

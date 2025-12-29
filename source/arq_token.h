#ifndef ARQ_TOKEN_H
#define ARQ_TOKEN_H

#include "arq_int.h"

typedef struct {
        uint32_t id;
        uint32_t size;
        char const *at;
} Arq_Token;

typedef struct {
        uint32_t num_of_token;
        uint32_t idx;
        Arq_Token at[1];
} Arq_OptVector;

typedef struct {
        uint32_t num_of_token;
        uint32_t idx;
        Arq_Token at[1];
} Arq_Vector;

typedef struct {
        uint32_t num_of_Vec;
        uint32_t row;
        Arq_OptVector *at[1];
} Arq_List;

#define ARQ_LIST_OF_IDS                          \
                                                 \
        /* helper to control tokenizer */        \
        X(ARQ_NO_TOKEN)                          \
                                                 \
        /* literals */                           \
        X(ARQ_P_DEC)                             \
        X(ARQ_N_DEC)                             \
        X(ARQ_DEC_FLOAT)                         \
        X(ARQ_HEX)                               \
        X(ARQ_HEX_FLOAT)                         \
        X(ARQ_CMD_RAW_STR)                       \
                                                 \
        /* option operators */                   \
        X(ARQ_OP_EQ)                             \
        X(ARQ_OP_COMMA)                          \
        X(ARQ_OP_ARRAY)                          \
        X(ARQ_OP_L_PARENTHESIS)                  \
        X(ARQ_OP_R_PARENTHESIS)                  \
        X(ARQ_OP_TERMINATOR)                     \
        X(ARQ_OP_UNKNOWN)                        \
                                                 \
        X(ARQ_IDENTFIER)                         \
        /* option keywords */                    \
        X(ARQ_NULL)                              \
        /* option types */                       \
        X(ARQ_TYPE_CSTR)                         \
        X(ARQ_TYPE_UINT)                         \
        X(ARQ_TYPE_ARRAY_SIZE)                   \
        X(ARQ_TYPE_INT)                          \
        X(ARQ_TYPE_FLOAT)                        \
                                                 \
        /* command line tokens */                \
        X(ARQ_CMD_SHORT_OPTION)                  \
        X(ARQ_CMD_LONG_OPTION)                   \
        X(ARQ_CMD_DASHDASH)                      \

#define X(name)name,
typedef enum {
        ARQ_LIST_OF_IDS
        end
} Arq_SymbolID;
#undef X

extern char const *symbol_names[];

#endif

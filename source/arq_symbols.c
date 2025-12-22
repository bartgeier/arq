#include "arq_token.h"


#define X(name) #name,
char const *symbol_names[] = {
        ARQ_LIST_OF_IDS
};
#undef X


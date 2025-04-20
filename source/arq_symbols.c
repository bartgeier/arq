#include "arq_symbols.h"


#define X(name) [name] = #name,
char *symbol_names[] = {
        ARQ_LIST_OF_IDS
};
#undef X


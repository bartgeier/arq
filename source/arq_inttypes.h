#ifndef ARQ_INTTYPES_H
#define ARQ_INTTYPES_H

#include "arq_int.h"

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <inttypes.h>
#else
    #define PRId32 "d"
    #define PRIu32 "u"
    #define PRId64 "ld"
    #define PRIu64 "lu"
#endif

#endif /* ARQ_INTTYPES_H */

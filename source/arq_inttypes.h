#ifndef ARQ_INTTYPES_H
#define ARQ_INTTYPES_H

#include "arq_int.h"

#if defined(__cplusplus) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    /* C++, >= C99 */
    #include <inttypes.h>
#else
    /* C89 */
    #define PRId8  "d"
    #define PRId32 "d"
    #define PRIu8  "u"
    #define PRIu32 "u"
    #define PRId64 "ld"
    #define PRIu64 "lu"
#endif

#endif /* ARQ_INTTYPES_H */

#if 1
#ifndef ARQ_INTTYPES_H
#define ARQ_INTTYPES_H

#include "arq_int.h"

#if defined(_MSC_VER) || \
    defined(__cplusplus) || \
   (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))

    #include <inttypes.h>

    #define ARQ_PRId8   PRId8
    #define ARQ_PRIu8   PRIu8
    #define ARQ_PRId16  PRId16
    #define ARQ_PRIu16  PRIu16
    #define ARQ_PRId32  PRId32
    #define ARQ_PRIu32  PRIu32

    #ifdef ARQ64
        #define ARQ_PRId64 PRId64
        #define ARQ_PRIu64 PRIu64
    #endif

#else /* C89 */

    #define ARQ_PRId8   "d"
    #define ARQ_PRIu8   "u"

    #define ARQ_PRId16  "d"
    #define ARQ_PRIu16  "u"

    #define ARQ_PRId32  "d"
    #define ARQ_PRIu32  "u"

    #ifdef ARQ64
        #define ARQ_PRId64 "ld"
        #define ARQ_PRIu64 "lu"
    #endif

#endif

#endif /* ARQ_INTTYPES_H */
#else
#ifndef ARQ_INTTYPES_H
#define ARQ_INTTYPES_H

#include "arq_int.h"

#if defined(_MSC_VER)
    #include <inttypes.h>
#elif defined(__cplusplus) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    /* C++, >= C99 */
    #include <inttypes.h>
#else
    /* C89 */
    #define PRId8  "d"
    #define PRId16 "d"
    #define PRId32 "d"
    #define PRIu8  "u"
    #define PRIu16 "u"
    #define PRIu32 "u"
    #define PRId64 "ld"
    #define PRIu64 "lu"
#endif

#endif /* ARQ_INTTYPES_H */
#endif

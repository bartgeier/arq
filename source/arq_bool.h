#ifndef ARQ_BOOL_H
#define ARQ_BOOL_H

/* If C99 or later, use the standard header */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <stdbool.h>
#else
    /* C89 fallback */
    typedef int bool;
    #define true 1
    #define false 0
#endif

#endif

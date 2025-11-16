#ifndef ARQ_BOOL_H
#define ARQ_BOOL_H

#if defined(__cplusplus) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    /* C++, >= C99 */
    #include <stdbool.h>
#else
    /* C89 */
    typedef int bool;
    #define true 1
    #define false 0
#endif

#endif

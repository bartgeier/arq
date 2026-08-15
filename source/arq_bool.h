#ifndef ARQ_BOOL_H
#define ARQ_BOOL_H

#if defined(_MSC_VER)
    #include <stdbool.h>
#elif defined(__cplusplus) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    /* C++, >= C99 */
    #include <stdbool.h>
    typedef bool arq_bool_t;
#else
    /* C89 */
    typedef int arq_bool_t;
#endif

#define ARQ_TRUE  ((arq_bool_t)1)
#define ARQ_FALSE ((arq_bool_t)0)

#endif

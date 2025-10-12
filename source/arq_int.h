
#ifndef ARQ_STDINT_H
#define ARQ_STDINT_H

/* If C99 or later, include the standard header */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <stdint.h>
#else
    /* C89 fallback definitions */

    /* Exact-width integer types */
    typedef signed char        int8_t;
    typedef unsigned char      uint8_t;

    typedef short              int16_t;
    typedef unsigned short     uint16_t;

    typedef int                int32_t;
    typedef unsigned int       uint32_t;

#if 0
#include <stddef.h>
    typedef size_t uintptr_t;
#endif

    /* 64-bit types, only if the compiler supports long long */
    #if defined(__GNUC__) || defined(_MSC_VER)
        /* typedef long long           int64_t;  */
        /* typedef unsigned long long  uint64_t; */
    #endif

    /* Optional: least types (at least N bits) */
    typedef int8_t  int_least8_t;
    typedef int16_t int_least16_t;
    typedef int32_t int_least32_t;
    #if defined(int64_t)
        typedef int64_t int_least64_t;
    #endif

    typedef uint8_t  uint_least8_t;
    typedef uint16_t uint_least16_t;
    typedef uint32_t uint_least32_t;
    #if defined(uint64_t)
        typedef uint64_t uint_least64_t;
    #endif

    /* Optional: define limits if needed */
    #define INT8_MIN   (-128)
    #define INT8_MAX   127
    #define UINT8_MAX  255

    #define INT16_MIN  (-32768)
    #define INT16_MAX  32767
    #define UINT16_MAX 65535

    #define INT32_MIN  (-2147483647 - 1)
    #define INT32_MAX  2147483647
    #define UINT32_MAX 4294967295U

    #if defined(int64_t)
        #define INT64_MIN  (-9223372036854775807LL - 1)
        #define INT64_MAX  9223372036854775807LL
        #define UINT64_MAX 18446744073709551615ULL
    #endif

#endif /* __STDC_VERSION__ >= 199901L */

#endif

#ifndef ARQ_STDINT_H
#define ARQ_STDINT_H

#include <stddef.h>  /* for size_t, ptrdiff_t */

/* ----------------- C99 detection ----------------- */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <stdint.h>
    #if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
        #define ARQ64
    #else
        #define ARQ32
    #endif
#else
    /* ----------------- Exact-width integer types ----------------- */
    typedef signed char        int8_t;
    typedef unsigned char      uint8_t;

    typedef short              int16_t;
    typedef unsigned short     uint16_t;

    typedef int                int32_t;
    typedef unsigned int       uint32_t;

    /* ----------------- Least-width types ----------------- */
    typedef int8_t   int_least8_t;
    typedef int16_t  int_least16_t;
    typedef int32_t  int_least32_t;

    typedef uint8_t  uint_least8_t;
    typedef uint16_t uint_least16_t;
    typedef uint32_t uint_least32_t;

    /* ----------------- Limits ----------------- */
    #define INT8_MIN   (-128)
    #define INT8_MAX   127
    #define UINT8_MAX  255

    #define INT16_MIN  (-32768)
    #define INT16_MAX  32767
    #define UINT16_MAX 65535

    #define INT32_MIN  (-2147483647 - 1)
    #define INT32_MAX  2147483647
    #define UINT32_MAX 4294967295U

    /* ----------------- 32-bit vs 64-bit detection ----------------- */
    #if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
        #define ARQ64
        #ifndef UINT64_T_DEFINED
            typedef size_t     uint64_t;
            typedef ptrdiff_t  int64_t;
            #define UINT64_T_DEFINED
        #endif
        #define UINT64_MAX ((size_t)-1)
        #define INT64_MAX  ((ptrdiff_t)(UINT64_MAX >> 1))
        #define INT64_MIN  (-INT64_MAX - 1)
    #else
        #define ARQ32
    #endif

    /* ----------------- Pointer-sized integer ----------------- */
    #ifndef UINTPTR_T_DEFINED
        #if defined(_MSC_VER)
            typedef unsigned __int64 uintptr_t;  /* MSVC 64-bit safe */
        #else
            typedef size_t uintptr_t;            /* GCC/Clang: pointer size */
        #endif
        #define UINTPTR_T_DEFINED
    #endif

#endif /* __STDC_VERSION__ >= 199901L */

#endif /* ARQ_STDINT_H */

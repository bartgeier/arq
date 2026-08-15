#ifndef ARQ_TYPES_H
#define ARQ_TYPES_H

#include <stddef.h> /* size_t, ptrdiff_t */

/* -------------------------------------------------------------
 * Architecture detection
 * ------------------------------------------------------------- */

#if defined(_WIN64) || defined(__x86_64__) || \
    defined(__aarch64__) || defined(__ppc64__)
    #define ARQ64
#else
    #define ARQ32
#endif

/* -------------------------------------------------------------
 * Fixed-width integer types
 * ------------------------------------------------------------- */

#if defined(_MSC_VER) || \
    defined(__cplusplus) || \
   (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))

    #include <stdint.h>

    typedef int8_t     arq_int8_t;
    typedef uint8_t    arq_uint8_t;
    typedef int16_t    arq_int16_t;
    typedef uint16_t   arq_uint16_t;
    typedef int32_t    arq_int32_t;
    typedef uint32_t   arq_uint32_t;

    #ifdef ARQ64
        typedef int64_t    arq_int64_t;
        typedef uint64_t   arq_uint64_t;
    #endif

#else /* C89 */

    typedef signed char        arq_int8_t;
    typedef unsigned char      arq_uint8_t;

    typedef short              arq_int16_t;
    typedef unsigned short     arq_uint16_t;

    typedef int                arq_int32_t;
    typedef unsigned int       arq_uint32_t;

    #ifdef ARQ64
        /* LP64: macOS/Linux | LLP64: size_t/ptrdiff_t are still 64-bit */
        typedef ptrdiff_t      arq_int64_t;
        typedef size_t         arq_uint64_t;
    #endif

#endif

/* -------------------------------------------------------------
 * Pointer-sized integer types
 * ------------------------------------------------------------- */

typedef ptrdiff_t  arq_intptr_t;
typedef size_t     arq_uintptr_t;

/* -------------------------------------------------------------
 * Integer limits
 * ------------------------------------------------------------- */

#define ARQ_INT8_MIN      (-128)
#define ARQ_INT8_MAX      127
#define ARQ_UINT8_MAX     255U

#define ARQ_INT16_MIN     (-32768)
#define ARQ_INT16_MAX     32767
#define ARQ_UINT16_MAX    65535U

#define ARQ_INT32_MIN     (-2147483647 - 1)
#define ARQ_INT32_MAX     2147483647
#define ARQ_UINT32_MAX    4294967295U

#ifdef ARQ64
    #define ARQ_UINT64_MAX ((arq_uint64_t)-1)
    #define ARQ_INT64_MAX  ((arq_int64_t)(ARQ_UINT64_MAX >> 1))
    #define ARQ_INT64_MIN  (-ARQ_INT64_MAX - 1)
#endif

#endif /* ARQ_TYPES_H */

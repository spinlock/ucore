#ifndef __LIBS_DEFS_H__
#define __LIBS_DEFS_H__

#ifndef NULL
#define NULL ((void *)0)
#endif

#define __always_inline inline __attribute__((always_inline))
#define __noinline __attribute__((noinline))
#define __noreturn __attribute__((noreturn))

/* Represents true-or-false values */
typedef int bool;

/* Explicitly-sized versions of integer types */
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

#if defined(__LP64__) || defined(__64BIT__) || defined(_LP64)
#ifndef __UCORE_64__
#define __UCORE_64__
#endif
#endif

/* *
 * We use pointer types to represent addresses,
 * uintptr_t to represent the numerical values of addresses.
 *
 * */

#ifdef __UCORE_64__

/* Pointers and addresses are 64 bits long in 64-bit platform. */
typedef int64_t intptr_t;
typedef uint64_t uintptr_t;

#else /* not __UCORE_64__ */

/* Pointers and addresses are 32 bits long in 32-bit platform. */
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

#endif /* !__UCORE_64__ */

/* size_t is used for memory object sizes. */
typedef uintptr_t size_t;

#endif /* !__LIBS_DEFS_H__ */


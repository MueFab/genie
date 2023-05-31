#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define _FILE_OFFSET_BITS 64

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// This might apparently be necessary to some broken C++ compiler :(
#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif

typedef uint8_t ch_t;       // Must be unsigned
#define CH_T_RANGE (UINT8_MAX+1)
typedef int16_t slch_t;     // Must be signed, and larger than "ch_t"

typedef uint32_t sa_t;      // Must be unsigned
typedef int32_t ssa_t;      // Must be signed - needed by suffixsort
#define SA_T_MAX UINT32_MAX
#define SSA_T_MAX INT32_MAX
#define SA_T_FMT "%d"

/*
typedef uint64_t sa_t;      // Must be unsigned
typedef int64_t ssa_t;      // Must be signed - needed by suffixsort
#define SA_T_MAX UINT64_MAX
#define SSA_T_MAX INT64_MAX
#define SA_T_FMT "%ld"
*/

#endif

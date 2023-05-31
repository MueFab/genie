#ifndef _UTILS_H_
#define _UTILS_H_

#include "typedef.h"

uint64_t ceil_div(const uint64_t n,const uint64_t m); /*DEC*/
uint8_t ceil_log2_uint6(const uint8_t x); /*DEC*/

int ceil_log2(const uint64_t x); /*DEC*/

int popcount64(const uint64_t n); /*DEC*/

typedef struct { /*DEC*/
  void* buf;
  size_t len;
  // If this one is true, then the buffer has been allocated somewhere else
  bool allocated;
} MB;

// If embedding!=0 we use it as an externally allocated buffer to store the object
MB* mb_new(const size_t size,void* const embedding); /*DEC*/
// It will fail if the buffer is embedded
void mb_write(const MB* const m,const char* const fname); /*DEC*/
// Returns a non-embedded buffer
MB* mb_read(const char* const fname); /*DEC*/
void mb_delete(MB* const m); /*DEC*/

#endif

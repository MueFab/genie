#ifndef _CC_H_GUARD_
#define _CC_H_GUARD_

#include "utils.h"

typedef struct { /*DEC*/
  // Vector length
  sa_t n;
  // Parameters
  ch_t bits_number;
  // Bitmap
  uint64_t* bitmap;
  // All the actual memory is stored here; everything else is just pointers
  MB* mem;
} CC;

// Returns the size in bytes
size_t cc_size(const sa_t n,const uint8_t bits_num); /*DEC*/

CC* cc_read(const char* filename,MB* const embedded_buffer); /*DEC*/

uint64_t cc_get(const CC* const cc,const sa_t pos); /*DEC*/

void cc_delete(CC* const cc); /*DEC*/

#endif

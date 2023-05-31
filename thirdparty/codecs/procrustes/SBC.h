#ifndef _SBC_H_GUARD_
#define _SBC_H_GUARD_

// Implements a bounded counter (i.e. such as c[i]<=i) sampled every s positions

#include "CC.h"

typedef struct { /*DEC*/
  // Vector full (unsampled) length
  sa_t n;
  // Parameters
  sa_t sampling_rate;
  // Multiplier between (minor) sampling rate and major sampling rate
  sa_t major_rate; // Optimisation - Not marshalled
  // Vectors
  CC* major;
  CC* minor;
  // All the actual memory is stored here; everything else is just pointers
  MB* mem;
} SBC;

// Returns the size in bytes
size_t sbc_size(const sa_t n,const sa_t sampling_rate);

SBC* sbc_new(const sa_t n,const sa_t sampling_rate,
             // As in this case objects are embedded at the same level rather than vertically,
             //  the interface is different - if a buffer is present we do not allocate memory
             MB* const embedding_buffer);

void sbc_write(const SBC* const sbc,const char* const filename);
SBC* sbc_read(const char* filename,MB* const embedded_buffer); /*DEC*/

void sbc_set(SBC* const sbc,const sa_t pos,const sa_t val);
sa_t sbc_get(const SBC* const sbc,const sa_t pos); /*DEC*/

void sbc_delete(SBC* const sbc); /*DEC*/

#endif

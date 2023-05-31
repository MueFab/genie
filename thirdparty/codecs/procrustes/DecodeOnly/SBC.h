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

SBC* sbc_read(const char* filename,MB* const embedded_buffer); /*DEC*/

sa_t sbc_get(const SBC* const sbc,const sa_t pos); /*DEC*/

void sbc_delete(SBC* const sbc); /*DEC*/

#endif

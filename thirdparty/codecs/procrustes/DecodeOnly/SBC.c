#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SBC.h"

#define SBC_SIZE_IN_BYTES 2*sizeof(sa_t)+size_minor+size_major
#define SBC_MAKE_SIZES \
  const sa_t major_rate=sampling_rate*(sa_t)(16.*log(n)/log(sampling_rate)+0.5); \
  const sa_t size_minor=cc_size(ceil_div(n,sampling_rate),ceil_log2(major_rate)); \
  const sa_t size_major=cc_size(ceil_div(n,major_rate),ceil_log2(n)); \
  assert((SBC_SIZE_IN_BYTES)%8==0)

SBC* sbc_read(const char* filename,MB* const embedded_buffer) { /*DEC*/
  if (filename&&embedded_buffer)
    errx(1,"(sbc_read) You must specify either a file or a buffer, not both");
  // We allocate the result
  SBC* res=(SBC*)calloc(1,sizeof(SBC));
  if (!res)
    err(1,"(sbc_read) Could not allocate handler");
  res->mem=(filename?mb_read(filename):embedded_buffer);
  // We distribute memory
  void* base=res->mem->buf;
  const sa_t n=*(sa_t*)base;
  res->n=n;
  base+=sizeof(sa_t);
  const sa_t sampling_rate=*(sa_t*)base;
  res->sampling_rate=sampling_rate;
  base+=sizeof(sa_t);
  SBC_MAKE_SIZES;
  res->major_rate=major_rate;
  res->major=cc_read(0,mb_new(size_major,base));
  base+=size_major;
  res->minor=cc_read(0,mb_new(size_minor,base));
  return res;  
}

sa_t sbc_get(const SBC* const sbc,const sa_t pos) { /*DEC*/
  return cc_get(sbc->major,pos/sbc->major_rate)+cc_get(sbc->minor,pos/sbc->sampling_rate);
}

void sbc_delete(SBC* const sbc) { /*DEC*/
  cc_delete(sbc->major);
  cc_delete(sbc->minor);
  mb_delete(sbc->mem);
  free(sbc);
  return;  
}

#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SBC.h"

// Returns the size in bytes
size_t sbc_size(const sa_t n,const sa_t sampling_rate) {
  // This is always 64-bit aligned (cc_size is)
#define SBC_SIZE_IN_BYTES 2*sizeof(sa_t)+size_minor+size_major
#define SBC_MAKE_SIZES \
  const sa_t major_rate=sampling_rate*(sa_t)(16.*log(n)/log(sampling_rate)+0.5); \
  const sa_t size_minor=cc_size(ceil_div(n,sampling_rate),ceil_log2(major_rate)); \
  const sa_t size_major=cc_size(ceil_div(n,major_rate),ceil_log2(n)); \
  assert((SBC_SIZE_IN_BYTES)%8==0)
  //
  SBC_MAKE_SIZES;
#ifdef PRINT_SIZES_AND_LOG
  printf("(sbc_size): Compressed size is %ld B (header=%ld,major="SA_T_FMT",minor="SA_T_FMT")\n",
         SBC_SIZE_IN_BYTES,4*sizeof(sa_t),size_major,size_minor);
#endif
  return SBC_SIZE_IN_BYTES;
}

SBC* sbc_new(const sa_t n,const sa_t sampling_rate,
             // As in this case objects are embedded at the same level rather than vertically,
             //  the interface is different - if a buffer is present we do not allocate memory
             MB* const embedding_buffer) {
  assert(n>0&&sampling_rate>0);
  // We allocate and initialise the result
  SBC* res=(SBC*)calloc(1,sizeof(SBC));
  if (!res)
    err(1,"(sbc_new) Could not allocate handler");
  res->n=n;
  res->sampling_rate=sampling_rate;
  SBC_MAKE_SIZES;
  res->major_rate=major_rate;
  // We allocate memory
  res->mem=embedding_buffer?embedding_buffer:mb_new(SBC_SIZE_IN_BYTES,0);
  // We distribute memory
  void* base=res->mem->buf;
  // Header
  *(sa_t*)base=res->n;
  base+=sizeof(sa_t);
  *(sa_t*)base=res->sampling_rate;
  base+=sizeof(sa_t);
  // Compressed vectors
  res->major=cc_new(ceil_div(n,major_rate),ceil_log2(n),mb_new(size_major,base));
  base+=size_major;
  res->minor=cc_new(ceil_div(n,sampling_rate),ceil_log2(major_rate),mb_new(size_minor,base));
  return res;
}

void sbc_write(const SBC* const sbc,const char* const filename) {
  if (!sbc->mem->allocated)
    errx(1,"(sbc_write) This function should never be called on an embedded object");
  // We write the data to the specified file
  mb_write(sbc->mem,filename);
  return;
}

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

void sbc_set(SBC* const sbc,const sa_t pos,const sa_t val) {
  assert(pos<sbc->n&&pos%sbc->sampling_rate==0&&val<=pos);
  if (pos%sbc->major_rate==0)
    cc_set(sbc->major,pos/sbc->major_rate,val);
  if (pos%sbc->sampling_rate==0)
    cc_set(sbc->minor,pos/sbc->sampling_rate,val-cc_get(sbc->major,pos/sbc->major_rate));
  return;
}
sa_t sbc_get(const SBC* const sbc,const sa_t pos) { /*DEC*/
  assert(pos<sbc->n&&pos%sbc->sampling_rate==0);
  return cc_get(sbc->major,pos/sbc->major_rate)+cc_get(sbc->minor,pos/sbc->sampling_rate);
}

void sbc_delete(SBC* const sbc) { /*DEC*/
  cc_delete(sbc->major);
  cc_delete(sbc->minor);
  mb_delete(sbc->mem);
  free(sbc);
  return;  
}

#undef SBC_SIZE_IN_BYTES
#undef SBC_MAKE_SIZES

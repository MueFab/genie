#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "CC.h"

// Returns the size in bytes
size_t cc_size(const sa_t n,const uint8_t bits_num) { /*DEC*/
  // This is always 64-bit aligned
#define CC_SIZE_IN_BYTES 2*sizeof(sa_t)+ceil_div(bits_num*n,64)*8
  return CC_SIZE_IN_BYTES;
}

CC* cc_new(const sa_t n,const uint8_t bits_num,
           // As in this case objects are embedded at the same level rather than vertically,
           //  the interface is different - if a buffer is present we do not allocate memory
           MB* const embedding_buffer) {
  assert(n>0);
  // We allocate and initialise the result
  CC* res=(CC*)calloc(1,sizeof(CC));
  if (!res)
    err(1,"(cc_new) Could not allocate handler");
  res->n=n;
  res->bits_number=bits_num;
  // We allocate memory
  res->mem=embedding_buffer?embedding_buffer:mb_new(CC_SIZE_IN_BYTES,0);
  // We distribute memory
  void* base=res->mem->buf;
  // Header
  *(sa_t*)base=res->n;
  base+=sizeof(sa_t);
  *(sa_t*)base=res->bits_number;
  base+=sizeof(sa_t);
  // Bitmap
  res->bitmap=base;
  //
  return res;
}

void cc_write(const CC* const cc,const char* const filename) {
  if (!cc->mem->allocated)
    errx(1,"(cc_write) This function should never be called on an embedded object");
  // We write the data to the specified file
  mb_write(cc->mem,filename);
  return;
}

CC* cc_read(const char* filename,MB* const embedded_buffer) { /*DEC*/
  if (filename&&embedded_buffer)
    errx(1,"(cc_read) You must specify either a file or a buffer, not both");
  // We allocate the result
  CC* res=(CC*)calloc(1,sizeof(CC));
  if (!res)
    err(1,"(cc_read) Could not allocate handler");
  res->mem=(filename?mb_read(filename):embedded_buffer);
  // We distribute memory
  void* base=res->mem->buf;
  res->n=*(sa_t*)base;
  base+=sizeof(sa_t);
  res->bits_number=*(sa_t*)base;
  base+=sizeof(sa_t);
  res->bitmap=base;
  //
  return res;  
}

void cc_set(CC* const cc,const sa_t pos,const uint64_t val) {
  // We left-align the value
  const uint64_t al_val=val<<(64-cc->bits_number);
  // We write the value at offset pos*bits_num.
  // Remember that al_val is left-aligned
  const uint64_t offset=pos*cc->bits_number;
  const uint64_t offset_div_bits=offset/64;
  const uint8_t offset_rem_bits=offset%64;
  cc->bitmap[offset_div_bits]|=(al_val>>offset_rem_bits);
  const uint8_t available=64-offset_rem_bits;
  if (available<cc->bits_number)
    cc->bitmap[offset_div_bits+1]|=(al_val<<available);
  return;
}
uint64_t cc_get(const CC* const cc,const sa_t pos) { /*DEC*/
  const uint64_t offset=pos*cc->bits_number;
  const uint64_t offset_div_bits=offset/64;
  const uint8_t offset_rem_bits=offset%64;
  uint64_t reg=(cc->bitmap[offset_div_bits]<<offset_rem_bits);
  const uint8_t available=64-offset_rem_bits;
  if (available<cc->bits_number)
    reg|=(cc->bitmap[offset_div_bits+1]>>available);
  // Remember that reg is left-aligned
  return reg>>(64-cc->bits_number);
}

void cc_delete(CC* const cc) { /*DEC*/
  mb_delete(cc->mem);
  free(cc);
  return;  
}

#undef CC_SIZE_IN_BYTES

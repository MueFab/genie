#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <assert.h>
#include <err.h>

#include "utils.h"
#include "CSA.h"

sa_t csa_lookup(const CSA* const a,const sa_t i0) { /*DEC*/
  if (i0>=a->bwt->n)
    errx(1,"(csa_lookup) Requested index "SA_T_FMT" is out of range (0-"SA_T_FMT")",i0,a->bwt->n-1);
  sa_t i=i0,dist=0;
  while (i%a->sampling_rate_sa) {
    i=bwt_LF(a->bwt,i);
    ++dist;
  }
  return ((sa_t)cc_get(a->sa,i/a->sampling_rate_sa)+dist)%a->bwt->n;
}

sa_t csa_inverse(const CSA* const a,const sa_t i0) { /*DEC*/
  if (i0>=a->bwt->n)
    errx(1,"(csa_inverse) Requested index "SA_T_FMT" is out of range (0-"SA_T_FMT")",i0,a->bwt->n-1);
  if (i0==0)
    return a->bwt->red_isa_0;
  const sa_t opt=a->bwt->n-i0;
  sa_t dist=opt%a->sampling_rate_lf,i=(sa_t)cc_get(a->lf,opt/a->sampling_rate_lf);
  while (dist>0) {
    i=bwt_LF(a->bwt,i);
    --dist;
  }
  return i;
}

#define MAKE_SIZES \
  size_t sa_size=cc_size(ceil_div(n,a->sampling_rate_sa),ceil_log2(n)); \
  size_t lf_size=cc_size(ceil_div(n,a->sampling_rate_lf),ceil_log2(n)); \
  size_t total_size=4*sizeof(sa_t)+sizeof(a->encode)+sizeof(a->decode)+sa_size+lf_size

CSA* csa_read(const char* const filename,MB* embedded_buffer) { /*DEC*/
  if (filename&&embedded_buffer)
    errx(1,"(csa_read) You must specify either a file or a buffer, not both");
  // We allocate the result
  CSA* a=(CSA*)calloc(1,sizeof(CSA));
  if (!a)
    err(1,"(csa_read) Could not allocate handler");
  a->mem=(filename?mb_read(filename):embedded_buffer);
  // We slice memory
  void* buf=a->mem->buf;
  const sa_t n=*(sa_t*)buf;
  buf+=sizeof(sa_t);
  a->alpha=*(sa_t*)buf;
  buf+=sizeof(sa_t);
  a->sampling_rate_sa=*(sa_t*)buf;
  buf+=sizeof(sa_t);
  a->sampling_rate_lf=*(sa_t*)buf;
  buf+=sizeof(sa_t);
  memcpy(a->encode,buf,sizeof(a->encode));
  buf+=sizeof(a->encode);
  memcpy(a->decode,buf,sizeof(a->decode));
  buf+=sizeof(a->decode);
  // We rebuild the counters
  MAKE_SIZES;
  a->sa=cc_read(0,mb_new(sa_size,buf));
  a->lf=cc_read(0,mb_new(lf_size,buf+sa_size));
  // We pass the rest of the buffer to wt_read
  a->bwt=wt_read(0,mb_new(a->mem->len-total_size,a->mem->buf+total_size));
  assert(a->bwt->n==n);
  return a;
}

void csa_delete(CSA* const a) { /*DEC*/
  cc_delete(a->sa);
  cc_delete(a->lf);
  wt_delete(a->bwt);
  // The only dynamic memory is in the memory map
  mb_delete(a->mem);
  // We deallocate the handler itself
  free(a);
  return;
}

// If terminate==true the buffer must accommodate len+1 characters,
//  so that the last one can be automatically zero-padded
MB* csa_decode(const CSA* const a,const sa_t i0,const sa_t len,const bool terminate) { /*DEC*/
  if (i0+len>a->bwt->n)
    errx(1,"(csa_decode) Parameters ("SA_T_FMT","SA_T_FMT") are out of range (0-"SA_T_FMT")",i0,len,a->bwt->n);
  MB* res=mb_new(len+(terminate?1:0),0);
  char* p=(char*)res->buf;
  const sa_t opt=i0+len;
  sa_t i=csa_inverse(a,opt==a->bwt->n?0:opt),j;
  for (j=0;j<len;++j) {
    p[len-1-j]=a->decode[wt_char(a->bwt,i)];
    i=bwt_LF(a->bwt,i);
  }
  if (terminate)
    p[len]=0;
  return res;
}

MB* csa_search(const CSA* const a,const ch_t* const key,const sa_t keylen) { /*DEC*/
  sa_t lo=0,hi=a->bwt->n,rlen=keylen;
  slch_t c;
  while (rlen-->0 /*&& hi>lo*/ ) {
    if ((c=a->encode[key[rlen]])==-1||hi==lo) {
      return mb_new(0,0);
    }
    // At this point for sure hi>0, and c>0
    lo=bwt_ELF(a->bwt,c,lo);
    hi=bwt_ELF(a->bwt,c,hi);
  }
  MB* res=mb_new((hi-lo)*sizeof(sa_t),0);
  sa_t i,* res_buf=(sa_t*)res->buf;
  for (i=lo;i<hi;++i)
    res_buf[i-lo]=csa_lookup(a,i);
  return res;
}

#undef MAKE_SIZES

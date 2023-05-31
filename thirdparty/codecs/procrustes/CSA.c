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

#include "qsufsort.h"
#include "utils.h"
#include "CSA.h"

// An auxiliary structure needed for sorting symbols during creation
typedef struct {
  ch_t symb;
  sa_t occs;
} SymbolStats;

int compare_stats(const void* const stat_1,const void* const stat_2) {
  const sa_t occs_1=((SymbolStats*)stat_1)->occs,occs_2=((SymbolStats*)stat_2)->occs;
  return (occs_1<occs_2)-(occs_1>occs_2);
}

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

CSA* csa_new(const sa_t n,ch_t (*init)(const sa_t),
             const uint64_t sampling_rate_sa,const uint64_t sampling_rate_lf,
             const sa_t superblock_size,const uint8_t block_size,
             const bool in_memory,const bool verbose) {
  if (sampling_rate_sa<1||sampling_rate_lf<1)
    errx(1,"(csa_new) Sampling rates must be positive numbers");
  // We allocate the result
  CSA* a=(CSA*)calloc(1,sizeof(CSA));
  if (!a)
    err(1,"(csa_new) Could not allocate handler");
  // We initialize the structure
  a->sampling_rate_sa=sampling_rate_sa;
  a->sampling_rate_lf=sampling_rate_lf;
  // Space for n, alpha, sampling_rate, decode, encode, lf
#define MAKE_SIZES \
  size_t sa_size=cc_size(ceil_div(n,a->sampling_rate_sa),ceil_log2(n)); \
  size_t lf_size=cc_size(ceil_div(n,a->sampling_rate_lf),ceil_log2(n)); \
  size_t total_size=4*sizeof(sa_t)+sizeof(a->encode)+sizeof(a->decode)+sa_size+lf_size
  //
  MAKE_SIZES;
  // We re-encode the original text
  SymbolStats stats[CH_T_RANGE]; // Auxiliary array
  sa_t i;
  for (i=0;i<CH_T_RANGE;++i) {
    stats[i].symb=i;
    stats[i].occs=0;
  }
  for (i=0;i<n;++i)
    ++(stats[init(i)].occs);
  // We sort symbols by decreasing frequency
  qsort(stats,sizeof(stats)/sizeof(SymbolStats),sizeof(SymbolStats),compare_stats);
  for (i=0;i<CH_T_RANGE;++i) {
    a->encode[i]=-1;
    a->decode[i]=0;
  }
  a->alpha=0;
  for (i=0;i<CH_T_RANGE;++i)
    if (stats[i].occs) {
      a->encode[stats[i].symb]=a->alpha;
      a->decode[a->alpha]=stats[i].symb;
      ++a->alpha;
    }
  if (verbose) {
    printf("CSA length is "SA_T_FMT"\n",n);
    printf("Characters are:\n");
    for (i=0;i<CH_T_RANGE;++i)
      if (stats[i].occs) {
        const ch_t symbol=stats[i].symb;
        const slch_t encoded=a->encode[symbol];
        printf(" %d ('%c'), "SA_T_FMT" time(s), encoded as %d\n",
               symbol,(unsigned char)symbol,stats[i].occs,encoded);
      }
    printf("(present="SA_T_FMT").\n",a->alpha);
    fflush(stdout);
  }
  // At this point we can compute the BWT of the encoded string
  sa_t red_isa_0;
  if (in_memory) {
#ifdef PRINT_SIZES_AND_LOG
    printf("(csa_new): Computing suffix array in memory...");
    fflush(stdout);
#endif
    // We create the uncompressed suffix array in memory (ugly)
    // Remember that for some reason suffixsort wants the array to be one element longer
    MB* mem_X=mb_new((n+1)*sizeof(ssa_t),0);
    ssa_t* X=(ssa_t*)mem_X->buf;
    for (i=0;i<n;++i) {
      X[i]=a->encode[init(i)];
      assert(X[i]>=0&&X[i]<a->alpha);
    }
    // Same here (1 element more)
    MB* mem_sa=mb_new((n+1)*sizeof(ssa_t),0);
    ssa_t* sa=(ssa_t*)mem_sa->buf;
    suffixsort(X,sa,n,0,CH_T_RANGE); // This means that the max symbol will be CH_T_RANGE-1
#ifdef PRINT_SIZES_AND_LOG
    printf(" done.\n");
    fflush(stdout);
#endif
    assert(sa[0]==n);
    // When using suffixsort, X==ISA, and hence X[0]==ISA[0]
    // The -1 comes from having removed \0 after construction
    red_isa_0=X[0]-1;
    mb_delete(mem_X);
    // We encode the BWT after discarding its terminator.
    // In order to do so, we have to determine ISA[0], or i_0 such that SA[i_0]==0.
    ch_t bwt_init(const sa_t ii) { return a->encode[init(ii==red_isa_0?n-1:sa[ii+1]-1)]; }
    // We do not actually allocate anything at this level, but rather ask wt_new to do that for us
    a->mem=mb_new(0,0);
    a->bwt=wt_new(n,bwt_init,red_isa_0,superblock_size,block_size,verbose,total_size,a->mem);
    // We test the correctness of character reconstruction
    for (i=0;i<n;++i)
      assert((wt_char(a->bwt,i)==bwt_init(i))||(
             printf("ERROR: pos="SA_T_FMT" wt_char=%d, init=%d\n",
                    i,wt_char(a->bwt,i),bwt_init(i))&&fflush(stdout)));
    mb_delete(mem_sa);
  } else
    errx(1,"(csa_new) On-disk BWT creation not yet implemented, sorry");
  void* buf=a->mem->buf;
  // We have to store n again even if it is already in the WT because we will need it to find the WT
  *(sa_t*)buf=n;
  buf+=sizeof(sa_t);
  *(sa_t*)buf=a->alpha;
  buf+=sizeof(sa_t);
  *(sa_t*)buf=a->sampling_rate_sa;
  buf+=sizeof(sa_t);
  *(sa_t*)buf=a->sampling_rate_lf;
  buf+=sizeof(sa_t);
  memcpy(buf,a->encode,sizeof(a->encode));
  buf+=sizeof(a->encode);
  memcpy(buf,a->decode,sizeof(a->decode));
  buf+=sizeof(a->decode);
  // We build the counters
  a->sa=cc_new(ceil_div(n,a->sampling_rate_sa),ceil_log2(n),mb_new(sa_size,buf));
  a->lf=cc_new(ceil_div(n,a->sampling_rate_lf),ceil_log2(n),mb_new(lf_size,buf+sa_size));
#ifdef PRINT_SIZES_AND_LOG
  printf("(csa_new): Compressed size is %ld B (header=%ld,SA=%ld,LF=%ld,WT=%ld)\n",
         a->mem->len,4*sizeof(sa_t)+sizeof(a->encode)+sizeof(a->decode),sa_size,lf_size,a->bwt->mem->len);
  fflush(stdout);
  printf("(csa_new): Generating CSA...");
  fflush(stdout);
#endif
  // We sample the LF space and test the correctness of LF-mapping
  sa_t lf=red_isa_0;
  for (i=0;i<n;++i) {
    if (!(lf%a->sampling_rate_sa))
      cc_set(a->sa,lf/a->sampling_rate_sa,n-i);
    if (!(i%a->sampling_rate_lf))
      cc_set(a->lf,i/a->sampling_rate_lf,lf);
    lf=bwt_LF(a->bwt,lf);
  }
  assert(lf==red_isa_0);
#ifdef PRINT_SIZES_AND_LOG
  printf(" done.\n");
  fflush(stdout);
#endif
  for (i=0;i<n;++i)
    assert(csa_inverse(a,csa_lookup(a,i))==i||(
           printf("ERROR: SA["SA_T_FMT"]="SA_T_FMT", ISA[SA["SA_T_FMT"]]="SA_T_FMT"\n",
                  i,csa_lookup(a,i),i,csa_inverse(a,csa_lookup(a,i)))&&fflush(stdout)));
  //
  return a;
}

void csa_write(const CSA* const a,const char* const filename) {
  if (!a->mem->allocated)
    errx(1,"(csa_write) This function should never be called on an embedded object");
  // We write the data to the specified file
  mb_write(a->mem,filename);
  return;
}

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

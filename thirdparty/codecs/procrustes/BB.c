#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "BB.h"

// Pre-computed tables
#include "binomials63.c"

//#define DEBUG_BB_ENCODE_BLOCK
//#define DEBUG_BB_DECODE_BLOCK
//#define DEBUG_BB_NEW
//#define DEBUG_BB_RANK_BIT
//#define PROFILE_BB_RANK_BIT

// Helper functions

/*
inline uint8_t num_bits_of_class(const uint8_t block_size,const uint8_t ones) {
  assert(block_size>0&&block_size<64&&ones<=block_size);
  // The table contains ceil(log2(binom(block_size,class)))
  //  for block_size=(0)..63 and class=0..floor(block_size/2)
  return bi_bits_of_ones[block_size][ones>block_size/2?block_size-ones:ones];
}
*/

// Bits are supposed to be aligned to the left of the block

inline static void bi_encode_block(const uint8_t block_size,const uint64_t al_block,
                                   uint8_t* al_ones,uint64_t* al_encoded) {
  assert(block_size>0&&block_size<64);
  // We zero the bits outside the block, and compute the class
  const uint8_t delta=64-block_size;
  const uint64_t mask=((0xffffffffffffffffULL>>delta)<<delta);
  const uint8_t ones=popcount64(al_block&mask);
  uint8_t class=(ones>block_size/2?block_size-ones:ones);
  // If the class is different from the number of ones, we invert the block
  uint64_t iblock=al_block;
  if (class!=ones)
    iblock=((0xffffffffffffffffULL-iblock)&mask);
#ifdef DEBUG_BB_ENCODE_BLOCK
  printf("> delta=%d, in=%016lx, masked=%016lx, ones=%d, class=%d, bits=%d, iblock=%016lx\n",
         delta,al_block,al_block&mask,ones,class,bi_bits_of_ones[block_size][class],iblock);
#endif
  // We encode the block
  const uint8_t red_block_size=block_size-1;
  int c=class,i;
  uint64_t res=0;
  for (i=0;i<block_size;++i)
    if ((iblock<<i)&0x8000000000000000ULL) {
      const uint8_t inv_i=red_block_size-i;
#ifdef DEBUG_BB_ENCODE_BLOCK
      printf("> %d([%d %d]=%ld)",i,red_block_size-i,c,inv_i>=c?bi_binomials[inv_i][c]:0);
#endif
      res+=(inv_i>=c?bi_binomials[inv_i][c]:0);
      c--;
    }
#ifdef DEBUG_BB_ENCODE_BLOCK
  printf("\n> res=%016lx\n",res);
#endif
  assert(c==0);
  *al_ones=ones<<(8-ceil_log2_uint6(block_size));
  *al_encoded=res<<(64-bi_bits_of_ones[block_size][class]);
#ifdef DEBUG_BB_ENCODE_BLOCK
  printf("> enc=%016lx\n",*al_encoded);
#endif
  return;
}

inline static uint64_t bi_decode_block(const uint8_t block_size,const uint8_t ones,uint64_t encoded) { /*DEC*/
  assert(block_size>0&&block_size<64&&ones<=block_size/2);
#ifdef DEBUG_BB_DECODE_BLOCK
  printf("< ones=%d, encoded=%016lx\n<",ones,encoded);
#endif
  int o=ones,i;
  uint64_t enc=encoded,res=0ULL;
  // Here we reconstruct the block right-aligned
  for (i=block_size-1;i>=0;--i) {
    const uint64_t binom=(i>=o?bi_binomials[i][o]:0);
    if (enc>=binom) {
#ifdef DEBUG_BB_DECODE_BLOCK
      printf(" %d(%ld,%ld)",block_size-1-i,binom,enc);
      fflush(stdout);
#endif
      res|=(1ULL<<i);
      enc-=binom;
      --o;
      // Many more optimisations would be possible
      if (o==0&&enc==0)
        break;
    }
  }
#ifdef DEBUG_BB_DECODE_BLOCK
  printf("\n");
#endif
  assert((o==0&&enc==0)||(
         printf("ERROR: block_size=%d ones=%d o=%d enc=%ld\n",block_size,ones,o,enc)&&fflush(stdout)));
  return res;
}

// Computes the length of the compressed bitmap from the uncompressed bitmap
void bi_length(const sa_t superblock_size,const uint8_t block_size,
               const sa_t unc_length,const uint64_t* const bitmap,
               uint64_t* const num_blocks,uint64_t* const sbl_size,
               uint64_t* const cls_length,uint64_t* const cmp_length) {
  assert(superblock_size>0&&block_size>0&&block_size<64&&unc_length>0);
  const uint8_t delta=64-block_size;
  const uint64_t mask=((0xffffffffffffffffULL>>delta)<<delta);
  uint64_t pos=0,res_blocks=0,res_length=0;
  while (pos<unc_length) {
    uint8_t to_do=block_size;
    if (pos+to_do>unc_length)
      // The length of the last block might be shorter
      to_do=unc_length-pos;
    const uint64_t pos_div_64=pos>>6;
    const uint8_t pos_rem_64=pos&63ULL;
    uint64_t reg=(bitmap[pos_div_64]<<pos_rem_64);
    const uint8_t available=64-pos_rem_64;
    if (available<to_do)
      // We are across the boundary between two 64-bit blocks
      reg|=(bitmap[pos_div_64+1]>>available);
    const uint8_t ones=popcount64(reg&mask);
    ++res_blocks;
    // This one really is block_size, not to_do
    res_length+=bi_bits_of_ones[block_size][ones>block_size/2?block_size-ones:ones];
    pos+=to_do;
  }
  *num_blocks=res_blocks;
  *sbl_size=2*sbc_size(unc_length,superblock_size*block_size);
  *cls_length=res_blocks*ceil_log2_uint6(block_size); // Would be 1+cblog(block_size/2)
  *cmp_length=res_length;
  return;
}

// Returns the size in bytes
size_t bb_size(const sa_t n,const uint64_t* const unc_bitmap,
               const sa_t superblock_size,const uint8_t block_size) {
  // This is always 64-bit aligned (sbl_size is because sbc_size is)
#define BB_MAKE_SIZES \
  uint64_t num_blocks,sbl_size,cls_length,cmp_length; \
  bi_length(superblock_size,block_size,n,unc_bitmap,&num_blocks,&sbl_size,&cls_length,&cmp_length); \
  const size_t bb_size_in_bytes=4*sizeof(sa_t)+sbl_size+ceil_div(cls_length+cmp_length,64)*8; \
  assert(bb_size_in_bytes%8==0)
  //
  BB_MAKE_SIZES;
#ifdef PRINT_SIZES_AND_LOG
  printf("(bb_size): Compressed size is %ld B (header=%ld,superblock=%ld,bitmaps=%ld)\n",
         bb_size_in_bytes,4*sizeof(sa_t),sbl_size,(size_t)ceil_div(cls_length+cmp_length,64)*8);
  fflush(stdout);
#endif
  return bb_size_in_bytes;
}

BB* bb_new(const sa_t n,const uint64_t* const unc_bitmap,const sa_t superblock_size,const uint8_t block_size,
           MB* const embedding_buffer) {
  assert(superblock_size>0&&block_size>0&&block_size<64&&n>0);
  // We allocate and initialise the result
  BB* res=(BB*)calloc(1,sizeof(BB));
  if (!res)
    err(1,"(bb_new) Could not allocate handler");
  BB_MAKE_SIZES;
  res->n=n;
  res->superblock_size=superblock_size;
  res->block_size=block_size;
  // We allocate memory
  res->mem=embedding_buffer?embedding_buffer:mb_new(bb_size_in_bytes,0);
  // We distribute memory
  void* base=res->mem->buf;
  // Header
  *(sa_t*)base=res->n;
  base+=sizeof(sa_t);
  const sa_t tmp=(res->superblock_size<<8)|(sa_t)res->block_size;
  *(sa_t*)base=tmp;
  base+=sizeof(sa_t);
  *(sa_t*)base=sbl_size;   // Necessary to reconstruct
  base+=sizeof(sa_t);      //  the object when reading
  *(sa_t*)base=cls_length; //  from disk/memory
  base+=sizeof(sa_t);
  // Superblock counters
  assert((sbl_size/2)%8==0);
  res->superblock_ranks=sbc_new(n,superblock_size*block_size,mb_new(sbl_size/2,base));
  base+=(sbl_size/2); // 64-bit aligned because sbc_size is
  res->superblock_offsets=sbc_new(n,superblock_size*block_size,mb_new(sbl_size/2,base));
  base+=(sbl_size/2); // 64-bit aligned because sbc_size is
  res->classes=base;
  res->bitmap_offset=cls_length;
  // We initialise the structure
  const uint8_t delta=64-block_size;
  const uint8_t ones_bits=ceil_log2_uint6(block_size);
  uint64_t pos=0,res_ones=0,res_blocks=0,res_length=0;
  while (pos<n) {
    uint8_t to_do=block_size;
    if (pos+to_do>n)
      // The length of the last block might be shorter
      to_do=n-pos;
    const uint64_t pos_div_64=pos>>6;
    const uint8_t pos_rem_64=pos&63ULL;
    uint64_t reg=(unc_bitmap[pos_div_64]<<pos_rem_64);
    const uint8_t available=64-pos_rem_64;
    if (available<to_do)
      // We are across the boundary between two 64-bit blocks
      reg|=(unc_bitmap[pos_div_64+1]>>available);
    uint8_t al_ones;
    uint64_t al_encoded;
    bi_encode_block(block_size,reg,&al_ones,&al_encoded);
    const uint8_t ones=al_ones>>(8-ones_bits);
#ifdef DEBUG_BB_NEW
    if (pos<1000)
      printf("[E] in=%016lx to_do=%d reg=%016lx al_ones=%02x ones=%d al_encoded=%016lx, classes=%p\n",
             unc_bitmap[pos/64],to_do,reg,al_ones,ones,al_encoded,res->classes);
#endif
    // If necessary, we update the superblock
    if (res_blocks%superblock_size==0) {
      sbc_set(res->superblock_ranks,pos,res_ones);
      sbc_set(res->superblock_offsets,pos,res_length);
    }
    res_ones+=ones;
    // We write the encoded ones at offset res_blocks*ones_bits.
    // Remember that al_ones is left-aligned
    const uint64_t offset_ones=res_blocks*ones_bits;
    const uint64_t ones_div_64=offset_ones>>6;
    const uint8_t ones_rem_64=offset_ones&63ULL;
    const uint64_t al_ones_64=((uint64_t)al_ones)<<56; // al_ones is 8 bits
    res->classes[ones_div_64]|=(al_ones_64>>ones_rem_64);
    const uint8_t ones_available=64-ones_rem_64;
    if (ones_available<ones_bits)
      res->classes[ones_div_64+1]|=(al_ones_64<<ones_available);
#ifdef DEBUG_BB_NEW
    if (pos<1000)
      printf("[E] res_blocks=%ld ones_div_64=%ld ones_rem_64=%d classes=%016lx%016lx\n",
             res_blocks,ones_div_64,ones_rem_64,res->classes[ones_div_64],res->classes[ones_div_64+1]);
#endif
    ++res_blocks;
    // If necessary, we write the encoded block at offset res->bitmap_offset+res_length
    const uint8_t block_bits=bi_bits_of_ones[block_size][ones>block_size/2?block_size-ones:ones];
    if (block_bits>0) {
      const uint64_t offset_block=res->bitmap_offset+res_length;
      const uint64_t block_div_64=offset_block>>6;
      const uint8_t block_rem_64=offset_block&63ULL;
      res->classes[block_div_64]|=(al_encoded>>block_rem_64);
      const uint8_t block_available=64-block_rem_64;
      if (block_available<block_bits)
        res->classes[block_div_64+1]|=(al_encoded<<block_available);
#ifdef DEBUG_BB_RANK_BIT
      if (pos<1000) {
        printf("[E] block_div_64=%ld block_rem_64=%d block_bits=%d bitmap=%016lx%016lx reg=%016lx\n",
              block_div_64,block_rem_64,block_bits,res->classes[block_div_64],
              res->classes[block_div_64+1],reg);
        fflush(stdout);
      }
#endif
      res_length+=block_bits;
    }
    pos+=to_do;
  }
  return res;
}

void bb_write(const BB* const bb,const char* const filename) {
  if (!bb->mem->allocated)
    errx(1,"(bb_write) This function should never be called on an embedded object");
  // We write the data to the specified file
  mb_write(bb->mem,filename);
  return;
}

BB* bb_read(const char* filename,MB* const embedded_buffer) { /*DEC*/
  if (filename&&embedded_buffer)
    errx(1,"(bb_read) You must specify either a file or a buffer, not both");
  // We allocate the result
  BB* res=(BB*)calloc(1,sizeof(BB));
  if (!res)
    err(1,"(bb_read) Could not allocate handler");
  res->mem=(filename?mb_read(filename):embedded_buffer);
  // We read the header
  void* base=res->mem->buf;
  const sa_t n=*(sa_t*)base;
  res->n=n;
  base+=sizeof(sa_t);
  const sa_t tmp=*(sa_t*)base;
  const sa_t superblock_size=tmp>>8;
  res->superblock_size=superblock_size;
  const sa_t block_size=tmp&(sa_t)255;
  res->block_size=block_size;
  base+=sizeof(sa_t);
  const sa_t sbl_size=*(sa_t*)base;
  base+=sizeof(sa_t);
  const sa_t cls_length=*(sa_t*)base;
  base+=sizeof(sa_t);
  assert((sbl_size/2)%8==0);
  res->superblock_ranks=sbc_read(0,mb_new(sbl_size/2,base));
  base+=(sbl_size/2); // 64-bit aligned because sbc_size is
  res->superblock_offsets=sbc_read(0,mb_new(sbl_size/2,base));
  base+=(sbl_size/2); // 64-bit aligned because sbc_size is
  res->classes=base;
  res->bitmap_offset=cls_length;
  return res;  
}

#ifdef PROFILE_BB_RANK_BIT
#include <time.h>

clock_t acc_one=0,acc_two=0,acc_thr=0,acc_fou=0;
int bb_rank_bit_cntr=0;

void bb_rank_bit_stats_init() {
  acc_one=0;
  acc_two=0;
  acc_thr=0;
  acc_fou=0;
  bb_rank_bit_cntr=0;
  return;
}

#endif

// The computation of the rank is exclusive
void bb_rank_bit(const BB* const bb,const sa_t pos,sa_t* const rank,bool* const bit) { /*DEC*/
  assert(bb->superblock_size>0&&bb->block_size>0&&bb->block_size<64&&bb->n>0&&pos<bb->n);
#ifdef PROFILE_BB_RANK_BIT
  bool do_profiling=(bb_rank_bit_cntr++%10000==0);
  clock_t beg,end;
  if (do_profiling)
    beg=clock();
#endif
  const uint8_t ones_bits=ceil_log2_uint6(bb->block_size);
  const uint8_t delta=64-ones_bits;
  const uint64_t mask=((0xffffffffffffffffULL>>delta)<<delta);
  const sa_t res_blocks=pos/bb->block_size;
  const sa_t res_blocks_div_superblock_size=res_blocks/bb->superblock_size;
  const sa_t res_blocks_rem_superblock_size=res_blocks%bb->superblock_size;
  const sa_t idx=res_blocks_div_superblock_size*bb->superblock_size*bb->block_size;
  // Base rank
  sa_t res=sbc_get(bb->superblock_ranks,idx);
  // Base offset in the compressed bitmap
  sa_t bitmap_offset=bb->bitmap_offset+sbc_get(bb->superblock_offsets,idx);
#ifdef PROFILE_BB_RANK_BIT
  if (do_profiling) {
    end=clock();
    acc_one+=(end-beg);
    beg=clock();
  }
#endif
  // We cycle over the missing blocks
  sa_t classes_offset=ones_bits*res_blocks_div_superblock_size*bb->superblock_size;
  int i;
  for (i=0;i<res_blocks_rem_superblock_size;++i) {
    const uint64_t ones_div_64=classes_offset/64;
    const uint8_t ones_rem_64=classes_offset%64;
    uint64_t reg=(bb->classes[ones_div_64]<<ones_rem_64);
    const uint8_t ones_available=64-ones_rem_64;
    if (ones_available<ones_bits)
      reg|=(bb->classes[ones_div_64+1]>>ones_available);
    const uint8_t al_ones=reg>>56; // al_ones is 8 bits
    const uint8_t ones=al_ones>>(8-ones_bits);
    res+=ones;
    bitmap_offset+=bi_bits_of_ones[bb->block_size][ones>bb->block_size/2?bb->block_size-ones:ones];
    classes_offset+=ones_bits;
  }
#ifdef DEBUG_BB_RANK_BIT
  if (pos<100) {
    printf("[D] pre_rank="SA_T_FMT" bitmap_offset="SA_T_FMT" classes_offset="SA_T_FMT" classes=%p\n",
          res,bitmap_offset,classes_offset,bb->classes);
    fflush(stdout);
  }
#endif
#ifdef PROFILE_BB_RANK_BIT
  if (do_profiling) {
    end=clock();
    acc_two+=(end-beg);
    beg=clock();
  }
#endif
  // If necessary, we decode the next block
  const uint64_t ones_div_64=classes_offset/64;
  const uint8_t ones_rem_64=classes_offset%64;
  uint64_t reg=(bb->classes[ones_div_64]<<ones_rem_64);
  const uint8_t ones_available=64-ones_rem_64;
  if (ones_available<ones_bits)
    reg|=(bb->classes[ones_div_64+1]>>ones_available);
  const uint8_t al_ones=reg>>56; // al_ones is 8 bits
  const uint8_t ones=al_ones>>(8-ones_bits);
#ifdef DEBUG_BB_RANK_BIT
  if (pos<100) {
    printf("[D] ones_div_64=%ld ones_rem_64=%d classes=%016lx%016lx reg=%016lx ones=%d\n",
          ones_div_64,ones_rem_64,bb->classes[ones_div_64],bb->classes[ones_div_64+1],reg,ones);
    fflush(stdout);
  }
#endif
  const bool invert_block=(ones>bb->block_size/2);
  const uint8_t eff_ones=(invert_block?bb->block_size-ones:ones);
  const uint8_t eff_block_bits=bi_bits_of_ones[bb->block_size][eff_ones];
#ifdef PROFILE_BB_RANK_BIT
  if (do_profiling) {
    end=clock();
    acc_thr+=(end-beg);
    beg=clock();
  }
#endif
  if (eff_block_bits>0) {
    const uint64_t block_div_64=bitmap_offset/64;
    const uint8_t block_rem_64=bitmap_offset%64;
    reg=(bb->classes[block_div_64]<<block_rem_64);
    const uint8_t block_available=64-block_rem_64;
    if (block_available<eff_block_bits)
      reg|=(bb->classes[block_div_64+1]>>block_available); // Everything left-aligned so far
    // Everything right-aligned from now on
    uint64_t decoded=bi_decode_block(bb->block_size,eff_ones,reg>>(64-eff_block_bits));
#ifdef DEBUG_BB_RANK_BIT
    if (pos<100) {
      printf("[D] block_div_64=%ld block_rem_64=%d block_bits=%d bitmap=%016lx%016lx reg=%016lx "
             "decoded=%016lx\n",
             block_div_64,block_rem_64,eff_block_bits,bb->classes[block_div_64],bb->classes[block_div_64+1],
             reg,decoded);
      fflush(stdout);
    }
#endif
    decoded=(invert_block?(0xffffffffffffffffULL>>(64-bb->block_size))-decoded:decoded);
#ifdef DEBUG_BB_RANK_BIT
    if (pos<100) {
      printf("[D] invert_block=%s decoded=%016lx\n",invert_block?"Y":"N",decoded);
      fflush(stdout);
    }
#endif
    const uint8_t res_rem=pos%bb->block_size;
    *rank=res+popcount64(decoded>>(bb->block_size-res_rem));
    // As we want to include the bit, we have to increment res_rem by 1 (goes from 1 to block_size)
    *bit=((decoded&(0x1ULL<<(bb->block_size-(res_rem+1))))!=0);
  } else {
    *rank=res+(invert_block?pos%bb->block_size:0);
    *bit=invert_block;
  }
#ifdef PROFILE_BB_RANK_BIT
  if (do_profiling) {
    end=clock();
    acc_fou+=(end-beg);
  }
#endif
  return;
}

#ifdef PROFILE_BB_RANK_BIT
void bb_rank_bit_stats() {
  clock_t total=acc_one+acc_two+acc_thr+acc_fou;
  printf("(%.2gs=%.3g%%,%.2gs=%.3g%%,%.2gs=%.3g%%,%.2gs=%.3g%%)\n",
         10000.*acc_one/CLOCKS_PER_SEC,100.*acc_one/total,10000.*acc_two/CLOCKS_PER_SEC,100.*acc_two/total,
         10000.*acc_thr/CLOCKS_PER_SEC,100.*acc_thr/total,10000.*acc_fou/CLOCKS_PER_SEC,100.*acc_fou/total);
  fflush(stdout);
  return;
}
#endif

void bb_delete(BB* const bb) { /*DEC*/
  sbc_delete(bb->superblock_ranks);
  sbc_delete(bb->superblock_offsets);
  mb_delete(bb->mem);
  free(bb);
  return;  
}

#undef BB_MAKE_SIZES

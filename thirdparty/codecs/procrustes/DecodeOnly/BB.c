#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "BB.h"

// Pre-computed tables
#include "binomials63.c"

// Helper functions

// Bits are supposed to be aligned to the left of the block

inline static uint64_t bi_decode_block(const uint8_t block_size,const uint8_t ones,uint64_t encoded) { /*DEC*/
  assert(block_size>0&&block_size<64&&ones<=block_size/2);
  int o=ones,i;
  uint64_t enc=encoded,res=0ULL;
  // Here we reconstruct the block right-aligned
  for (i=block_size-1;i>=0;--i) {
    const uint64_t binom=(i>=o?bi_binomials[i][o]:0);
    if (enc>=binom) {
      res|=(1ULL<<i);
      enc-=binom;
      --o;
      // Many more optimisations would be possible
      if (o==0&&enc==0)
        break;
    }
  }
  return res;
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
  res->superblock_ranks=sbc_read(0,mb_new(sbl_size/2,base));
  base+=(sbl_size/2); // 64-bit aligned because sbc_size is
  res->superblock_offsets=sbc_read(0,mb_new(sbl_size/2,base));
  base+=(sbl_size/2); // 64-bit aligned because sbc_size is
  res->classes=base;
  res->bitmap_offset=cls_length;
  return res;  
}

// The computation of the rank is exclusive
void bb_rank_bit(const BB* const bb,const sa_t pos,sa_t* const rank,bool* const bit) { /*DEC*/
  assert(bb->superblock_size>0&&bb->block_size>0&&bb->block_size<64&&bb->n>0&&pos<bb->n);
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
  // If necessary, we decode the next block
  const uint64_t ones_div_64=classes_offset/64;
  const uint8_t ones_rem_64=classes_offset%64;
  uint64_t reg=(bb->classes[ones_div_64]<<ones_rem_64);
  const uint8_t ones_available=64-ones_rem_64;
  if (ones_available<ones_bits)
    reg|=(bb->classes[ones_div_64+1]>>ones_available);
  const uint8_t al_ones=reg>>56; // al_ones is 8 bits
  const uint8_t ones=al_ones>>(8-ones_bits);
  const bool invert_block=(ones>bb->block_size/2);
  const uint8_t eff_ones=(invert_block?bb->block_size-ones:ones);
  const uint8_t eff_block_bits=bi_bits_of_ones[bb->block_size][eff_ones];
  if (eff_block_bits>0) {
    const uint64_t block_div_64=bitmap_offset/64;
    const uint8_t block_rem_64=bitmap_offset%64;
    reg=(bb->classes[block_div_64]<<block_rem_64);
    const uint8_t block_available=64-block_rem_64;
    if (block_available<eff_block_bits)
      reg|=(bb->classes[block_div_64+1]>>block_available); // Everything left-aligned so far
    // Everything right-aligned from now on
    uint64_t decoded=bi_decode_block(bb->block_size,eff_ones,reg>>(64-eff_block_bits));
    decoded=(invert_block?(0xffffffffffffffffULL>>(64-bb->block_size))-decoded:decoded);
    const uint8_t res_rem=pos%bb->block_size;
    *rank=res+popcount64(decoded>>(bb->block_size-res_rem));
    // As we want to include the bit, we have to increment res_rem by 1 (goes from 1 to block_size)
    *bit=((decoded&(0x1ULL<<(bb->block_size-(res_rem+1))))!=0);
  } else {
    *rank=res+(invert_block?pos%bb->block_size:0);
    *bit=invert_block;
  }
  return;
}

void bb_delete(BB* const bb) { /*DEC*/
  sbc_delete(bb->superblock_ranks);
  sbc_delete(bb->superblock_offsets);
  mb_delete(bb->mem);
  free(bb);
  return;  
}

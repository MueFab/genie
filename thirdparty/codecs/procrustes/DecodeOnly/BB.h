#ifndef _BB_H_GUARD_
#define _BB_H_GUARD_

#include "SBC.h"

typedef struct { /*DEC*/
  // Text length
  sa_t n;
  // Parameters
  sa_t superblock_size; // We combine this and the following
  uint8_t block_size;   //  to preserve alignment
  // Offsets/pointers
  SBC* superblock_ranks;
  SBC* superblock_offsets;
  uint64_t* classes;
  sa_t bitmap_offset; // Offset of bitmap from classes
  // All the actual memory is stored here; everything else is just pointers
  MB* mem;
} BB;

BB* bb_read(const char* filename,MB* const embedded_buffer); /*DEC*/

// The computation of the rank is inclusive
void bb_rank_bit(const BB* const bb,const sa_t pos,sa_t* const rank,bool* const bit); /*DEC*/

void bb_delete(BB* const bb); /*DEC*/

#endif

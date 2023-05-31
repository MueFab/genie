#ifndef _WT_H_GUARD_
#define _WT_H_GUARD_

#include <stdbool.h>
#include "typedef.h"
#include "utils.h"
#include "BB.h"

typedef struct _WTNode_ { /*DEC*/
  ch_t min_char;
  ch_t max_char;
  bool is_terminal; // Optimisation - Not marshalled
  // Pointer to compressed bitmap - it contains size information
  BB* bitmap;
  // Pointer to children nodes
  struct _WTNode_* left;
  struct _WTNode_* right;
} WTNode;

typedef struct { /*DEC*/
  // Text length
  sa_t n;
  // Number of different symbols (not really needed, kept for reference).
  ch_t alpha;
  // The cumulative occurrences ("ranks") of the "alpha" symbols of the string
  //  as a function of their lexicographic order, with an index offset of 1
  //  so that c[3] gives occs(0)+occs(1)+occs(2).
  // For instance, if the string is "ebdebddaddebebdc\0" we get
  //   c[0]=0; c[1]=1; c[2]=2; c[3]=6; c[4]=7; c[5]=13; c[6]=17
  sa_t c[CH_T_RANGE+1];
  // We represent the BWT as a shortened one without terminator.
  // This is the original position of the terminator
  sa_t red_isa_0;
  // And this is the character which was originally present at that position.
  // It will be used to correctly compute modified ranks, i.e. the original ones
  ch_t char_red_isa_0;
  // The root of the tree
  WTNode* root;
  // All the actual memory is stored here; everything else is just pointers
  MB* mem;
} WT;

// Returns a non-embedded buffer, unless one is specified
WT* wt_read(const char *filename,MB* embedded_buffer); /*DEC*/
void wt_delete(WT* const wt); /*DEC*/

ch_t wt_char(WT* const wt,sa_t i); /*DEC*/

// The following two functions assume that the content is an actual BWT
// It holds true that BWT[LF[i]]=init[i-1], BWT[LF[i]^2]=init[i-2], etc.
sa_t bwt_LF(WT* const wt,sa_t i); /*DEC*/
sa_t bwt_ELF(WT* const wt,ch_t c,sa_t i); /*DEC*/

#endif

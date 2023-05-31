#ifndef _CSA_H_GUARD_
#define _CSA_H_GUARD_

#include "typedef.h"
#include "CC.h"
#include "WT.h"

typedef struct { /*DEC*/
  // Number of different symbols in the null-terminated string.
  ch_t alpha;
  // The mapping from characters in the string to the compressed range, that
  //  is: the lexicographic order for the "alpha" symbols in the string.
  // The remaining entries are padded to -1
  slch_t encode[CH_T_RANGE]; // Automatically aligned because there are CH_T_RANGE elements
  // The mapping from the compressed range/lexicographic order of the symbols
  //  (from 0 to "alpha"-1) to the original characters in the string.
  // For instance, if the string is "ebdebddaddebebdc\0" we get
  //   decode[0]=0 ('\0'), decode[1]=33 ('a'), decode[2]=34 ('b'),
  //   decode[3]=35 ('c'), decode[4]=36 ('d'), decode[5]=37 ('e')
  ch_t decode[CH_T_RANGE]; // Automatically aligned because there are CH_T_RANGE elements
  // Interval between two values of the SA stored explicitly
  sa_t sampling_rate_sa;
  // Interval between two values of LF stored explicitly
  sa_t sampling_rate_lf;
  // Pointer to SA data
  CC* sa;
  // Pointer to LF/ISA data
  CC* lf;
  // The BWT.
  // It only contains encoded characters
  WT* bwt;
  // The memory is actually stored in the following object
  MB* mem;
} CSA;

// Returns a non-embedded buffer
CSA* csa_read(const char* const filename,MB* embedded_buffer); /*DEC*/
void csa_delete(CSA* const a); /*DEC*/

MB* csa_search(const CSA* const a,const ch_t* const key,sa_t keylen); /*DEC*/
// Decode T[i..i+len-1]. If terminate==true, pad the result with a final \0
MB* csa_decode(const CSA* const a,const sa_t i,const sa_t len,const bool terminate); /*DEC*/

#endif

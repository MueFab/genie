/*

   qsufsort.h
   Copyright 1999, N. Jesper Larsson, all rights reserved.
             2008, P. Ribeca, all rights reserved.

   This file contains an implementation of the algorithm presented in "Faster
   Suffix Sorting" by N. Jesper Larsson (jesper@cs.lth.se) and Kunihiko
   Sadakane (sada@is.s.u-tokyo.ac.jp).

   This software may be used freely for any purpose. However, when distributed,
   the original source must be clearly stated, and, when the source code is
   distributed, the copiright notice must be retained and any alterations in
   the code must be clearly marked. No warranty is given regarding the quality
   of this software.

*/

#ifndef _QSUFSORT_H_GUARD_
#define _QSUFSORT_H_GUARD_

#include "typedef.h"

void suffixsort(ssa_t* x,ssa_t* p,ssa_t n,ssa_t l,ssa_t k);

#endif

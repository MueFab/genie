#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <err.h>

#include "WT.h"

#define DEFINE_SIZES \
  size_t total_memory_toplevel=5*sizeof(sa_t)+sizeof(res->c), \
    total_memory_headers=0,total_memory_bitmaps=0
#define SLICE_MEMORY \
  void* base_toplevel=res->mem->buf; \
  void* base_headers=base_toplevel+total_memory_toplevel; \
  void* base_bitmaps=base_headers+total_memory_headers

WT* wt_read(const char* filename,MB* embedded_buffer) { /*DEC*/
  if (filename&&embedded_buffer)
    errx(1,"(wt_read) You must specify either a file or a buffer, not both");
  // We allocate the result
  WT* res=(WT*)calloc(1,sizeof(WT));
  if (!res)
    err(1,"(wt_read) Could not allocate handler");
  res->mem=(filename?mb_read(filename):embedded_buffer);
  DEFINE_SIZES;
  // We read the header
  void* base=res->mem->buf;
  // Global header
  res->n=*(sa_t*)base;
  base+=sizeof(sa_t);
  res->alpha=*(sa_t*)base;
  base+=sizeof(sa_t);
  memcpy(res->c,base,sizeof(res->c));
  base+=sizeof(res->c);
  res->red_isa_0=*(sa_t*)base;
  base+=sizeof(sa_t);
  res->char_red_isa_0=*(sa_t*)base;
  base+=sizeof(sa_t);
  sa_t num_nodes=*(sa_t*)base; // We would not reallly need this here, but we use it to double check
  base+=sizeof(sa_t);
  // First we reconstruct the tree structure and compute the offsets of the different sections
  void wt_read_rec1(WTNode** father_ptr) {
    if (!(*father_ptr=(WTNode*)calloc(1,sizeof(WTNode))))
      err(1,"(wt_read_rec) Could not allocate node");
    WTNode* father=*father_ptr;
    --num_nodes;
    const sa_t tmp=*(sa_t*)base;
    base+=sizeof(sa_t);
    father->min_char=(tmp>>16)&((sa_t)0xffff);
    father->max_char=tmp&((sa_t)0xffff);
    const sa_t size=*(sa_t*)base;
    base+=sizeof(sa_t);
    father->is_terminal=(size==0); // Or min_char==max_char
    if (!father->is_terminal) {
      total_memory_bitmaps+=size;
      wt_read_rec1(&father->left);
      wt_read_rec1(&father->right);
    }
    return;
  }
  wt_read_rec1(&res->root);
  // Then we slice memory
  void* base_bitmaps=base;
  // We rewind the header, as we will need the sizes of the compressed bitmaps again
  base=res->mem->buf+total_memory_toplevel;
  void wt_read_rec2(WTNode** father_ptr) {
    WTNode* father=*father_ptr;
    // We skip the char range
    base+=sizeof(sa_t);
    const sa_t size=*(sa_t*)base;
    base+=sizeof(sa_t);
    if (!father->is_terminal) {
      // We rebuild the compressed bitmap...
      father->bitmap=bb_read(0,mb_new(size,base_bitmaps));
      base_bitmaps+=size;
      wt_read_rec2(&father->left);
      wt_read_rec2(&father->right);
    } else
      father->bitmap=0;
    return;
  }
  wt_read_rec2(&res->root);
  return res;
}

void wt_delete(WT* wt) { /*DEC*/
  void wt_delete_rec(WTNode* node) {
    if (node->left)
      wt_delete_rec(node->left);
    if (node->right)
      wt_delete_rec(node->right);
    if (node->bitmap)
      bb_delete(node->bitmap);
    free(node);
    return;
  }
  wt_delete_rec(wt->root);
  mb_delete(wt->mem);
  free(wt);
  return;
}

// Decodes WT[i]
ch_t wt_char(WT* const wt,sa_t i) { /*DEC*/
  if (i>=wt->n)
    errx(1,"(wt_char) Requested index "SA_T_FMT" is out of range",i);
  sa_t where=i;
  WTNode* node=wt->root;
  while (!node->is_terminal) {
    sa_t rank;
    bool bit;
    bb_rank_bit(node->bitmap,where,&rank,&bit);
    if (bit) {
      where=rank;
      node=node->right;
    } else {
      where-=rank;
      node=node->left;
    }
  }
  return node->min_char;
}

sa_t bwt_LF(WT* const wt,sa_t i) { /*DEC*/
  if (i>=wt->n)
    errx(1,"(bwt_LF) Requested index "SA_T_FMT" is out of range",i);
  sa_t where=i;
  WTNode* node=wt->root;
  while (!node->is_terminal) {
    sa_t rank;
    bool bit;
    bb_rank_bit(node->bitmap,where,&rank,&bit);
    if (bit) {
      where=rank;
      node=node->right;
    } else {
      where-=rank;
      node=node->left;
    }
  }
  // In this case we skip the original 0 and move directly to the original LF[0]
  if (i==wt->red_isa_0)
    return wt->c[node->min_char];
  // Here we need to correct for the ranks of the swapped element in the BWT
  if (node->min_char==wt->char_red_isa_0&&i<wt->red_isa_0)
    return wt->c[node->min_char]+where+1;
  // Here we just return the regular rank (by far the most frequent case)
  return wt->c[node->min_char]+where;
}

sa_t bwt_ELF(WT* const wt,ch_t c,sa_t i) { /*DEC*/
  if (i>wt->n||c>=wt->alpha)
    errx(1,"(bwt_ELF) Parameters (%d,"SA_T_FMT") are out of range",c,i);
  if (!i)
    return wt->c[c];
  if (i==wt->n)
    return wt->c[c+1];
  sa_t res=i;
  WTNode* node=wt->root;
  while (!node->is_terminal) {
    sa_t rank;
    bool tmp;
    bb_rank_bit(node->bitmap,res,&rank,&tmp);
    if (c==node->min_char) {
      res-=rank;
      node=node->left;
    } else {
      res=rank;
      node=node->right;
    }
  }
  return wt->c[c]+res;
}

#undef SIZE_UNC_BITMAP
#undef DEFINE_SIZES
#undef SLICE_MEMORY

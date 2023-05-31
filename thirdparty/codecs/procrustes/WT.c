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

//#define DEBUG_WT_NEW
//#define DEBUG_WT_RANK
//#define DEBUG_WT_CHAR
//#define DEBUG_WT_LF

// An auxiliary structure needed during tree creation to linearize it
typedef struct {
  sa_t bit_num;
  sa_t ones;
  // The uncompressed bitmap - used during construction
  sa_t bitmap_len; // Length in bits
  uint64_t* bitmap;
  WTNode* node;
} WTNodeAux;

WT* wt_new(sa_t n,ch_t (*init)(const sa_t),sa_t red_isa_0,
           const sa_t superblock_size,const uint8_t block_size,bool verbose,
           size_t embedding_excess,MB* embedding_buffer) {
  assert(n>=0); // This should always be true because sa_t is an unsigned type
  if (embedding_excess&&!embedding_buffer)
    errx(1,"(wt_new) The embedding buffer structure must have been already allocated");
  // We allocate the result
  WT* res=(WT*)calloc(1,sizeof(WT));
  if (!res)
    err(1,"(wt_new) Could not allocate handler");
  // We initialize the structure
  res->n=n;
  res->red_isa_0=red_isa_0;
  res->char_red_isa_0=init(red_isa_0);
  sa_t i;
  for (i=0;i<=CH_T_RANGE;++i)
    res->c[i]=0;
  if (verbose) {
//   for (i=0;i<n;++i) {
//     printf("c[%ld]=%d\n",i,init(i));
//     fflush(stdout);    
//   }
    printf("red_isa_0="SA_T_FMT"\n",red_isa_0);
    fflush(stdout);
    printf("c[red_isa_0]=%d\n",init(red_isa_0));
    fflush(stdout);
  }
  for (i=0;i<n;++i)
    ++res->c[init(i)];
  // We turn occurrencies into cumulative occurrencies
  res->alpha=0;
  if (res->c[0])
    ++res->alpha;
  sa_t save=res->c[0],old_save;
  res->c[0]=0;
  for (i=1;i<=CH_T_RANGE;++i) {
    if (res->c[i])
      ++res->alpha;
    old_save=save;
    save=res->c[i];
    res->c[i]=res->c[i-1]+old_save;
  }
  assert(save==0);
//   // We subtract 1 for the character that got swapped with the terminator
//   --res->c[init(red_isa_0)];
  // We create the nodes of the tree
  int depth=ceil_log2(res->alpha)+1;
  if (verbose) {
    printf("WT length is "SA_T_FMT"\n",res->n);
    printf("Tree depth is %d\n",depth);
    printf("Accumulated counters are:\n");
    for (i=0;i<res->alpha;++i)
      printf(" "SA_T_FMT", c["SA_T_FMT"]="SA_T_FMT"\n",i,i,res->c[i]);
    printf("(characters present="SA_T_FMT").\n",res->alpha);
    fflush(stdout);
  }
  WTNodeAux* node_list=(WTNodeAux*)calloc((1<<depth)-1,sizeof(WTNodeAux));
  if (verbose) {
    printf("Allocated %d nodes\n",(1<<depth)-1);
    fflush(stdout);
  }
  if (!node_list)
    err(1,"(wt_new) Could not allocate node list");
  if (!(res->root=(WTNode*)calloc(1,sizeof(WTNode))))
    err(1,"(wt_new) Could not allocate root node");
  WTNodeAux* node_aux;
  WTNode* node_aux_node;
  int node_num;
  void wt_new_rec(WTNode* node,ch_t min_char,ch_t max_char) {
    if (verbose)
      printf("[#%d] Character interval [%d-%d]\n",node_num-1,min_char,max_char);
    node->min_char=min_char;
    node->max_char=max_char;
    node->is_terminal=(min_char==max_char);
    node->bitmap=0;
    if (!node->is_terminal) {
      node_aux=node_list+node_num-1;
      node_aux->bitmap_len=res->c[max_char+1]-res->c[min_char];
#define SIZE_UNC_BITMAP(n) ceil_div(n,64)*sizeof(uint64_t)
      // We allocate the temporary uncompressed bitmap
      node_aux->bitmap=(uint64_t*)calloc(1,SIZE_UNC_BITMAP(node_aux->bitmap_len));
      node->left=(WTNode*)calloc(1,sizeof(WTNode));
      node->right=(WTNode*)calloc(1,sizeof(WTNode));
      if (!node->left||!node->right)
        err(1,"(wt_new_rec) Could not allocate children nodes");
      node_aux=node_list+(node_num++);
      node_aux->bit_num=0;
      node_aux->ones=0;
      node_aux->node=node->left;
      wt_new_rec(node->left,min_char,min_char);
      node_aux=node_list+(node_num++);
      node_aux->bit_num=0;
      node_aux->ones=0;
      node_aux->node=node->right;
      wt_new_rec(node->right,min_char+1,max_char);
    } else {
      node->left=0;
      node->right=0;
    }
    return;
  }
  node_list->bit_num=0;
  node_list->ones=0;
  node_list->node=res->root;
  node_num=1;
  wt_new_rec(res->root,0,res->alpha-1);
  // We build the bitmaps
  //  We find all the nodes where a given character appears
  int* char_to_nodes=(int*)calloc(node_num*res->alpha,sizeof(int)),
    * node_nums=(int*)calloc(res->alpha,sizeof(int));
  if (!char_to_nodes||!node_nums)
    err(1,"(wt_new) Could not allocate node catalogues");
  int j;
  for (i=0;i<node_num;++i)
    for (j=node_list[i].node->min_char;j<=node_list[i].node->max_char;++j)
      if (!node_list[i].node->is_terminal)
        char_to_nodes[j*node_num+(node_nums[j]++)]=i;
  if (verbose) {
    printf("char_to_nodes=[%p-%p]\n",char_to_nodes,char_to_nodes+node_num*res->alpha*sizeof(int));
    fflush(stdout);
  }
#ifdef DEBUG_WT_NEW
  for (i=0;i<res->alpha;++i) {
    printf("Char "SA_T_FMT": in non-terminal node(s)",i);
    for (j=0;j<node_nums[i];++j)
      printf(" %d",char_to_nodes[i*node_num+j]);
    printf("\n");
  }
  fflush(stdout);
#endif
  ch_t encoded;
  bool is_right;
  for (i=0;i<res->n;++i) {
    encoded=init(i);
#ifdef DEBUG_WT_NEW
    printf("Char %d:",encoded);
    fflush(stdout);
#endif
    for (j=0;j<node_nums[encoded];++j) {
      node_aux=node_list+char_to_nodes[encoded*node_num+j];
      node_aux_node=node_aux->node;
      is_right=(encoded>node_aux_node->min_char);
#ifdef DEBUG_WT_NEW
      printf(" Node=%ld(%s)",node_aux-node_list,is_right?">":"<");
      fflush(stdout);
#endif
      if (is_right) {
        // At the beginning memory is zero
        node_aux->bitmap[node_aux->bit_num>>6]|=1ULL<<(63-(node_aux->bit_num&63ULL));
        ++node_aux->ones;
      }
      // The tables are displaced by one point (arriving up to "bitmap_len"+1)
      ++node_aux->bit_num;
    }
#ifdef DEBUG_WT_NEW
    printf("\n");
    fflush(stdout);
#endif
  }
#ifdef DEBUG_WT_NEW
  for (j=0;j<node_num;++j) {
    node_aux=node_list+j;
    node_aux_node=(node_list+j)->node;
    printf("[#%d] ",j);
    if (node_aux_node->is_terminal)
      printf("Terminal\n");
    else {
      printf("Non-terminal:\n  Bitmap("SA_T_FMT"): ",node_aux->bitmap_len);
      for (i=0;i<node_aux->bitmap_len;++i)
        printf("%s",node_aux->bitmap[i/64]&(1ULL<<(63-i%64ULL))?"1":"0");
      printf("\n");
    }
  }
  fflush(stdout);
#endif
  //
#define DEFINE_SIZES \
  size_t total_memory_toplevel=5*sizeof(sa_t)+sizeof(res->c), \
    total_memory_headers=0,total_memory_bitmaps=0
  //
  DEFINE_SIZES;
  // We determine the sizes of the compressed bitmaps
  for (j=0;j<node_num;++j) {
    node_aux=node_list+j;
    node_aux_node=node_aux->node;
    total_memory_headers+=2*sizeof(sa_t);
    if (verbose)
      printf("[#%d] Character interval ["SA_T_FMT"-"SA_T_FMT"]",
            j,node_aux_node->min_char,node_aux_node->max_char);
    if (!node_aux_node->is_terminal) {
      const size_t size=bb_size(node_aux->bitmap_len,node_aux->bitmap,superblock_size,block_size);
      total_memory_bitmaps+=size;
      if (verbose) {
        printf(", size=%ld B)\n",size);
        fflush(stdout);
      }
    } else {
      if (verbose) {
        printf(" (terminal)\n");
        fflush(stdout);
      }
    }
  }
  // We allocate memory
  const size_t total_memory=
    total_memory_toplevel+ // For the global header
    total_memory_headers+total_memory_bitmaps;
  res->mem=mb_new(total_memory+embedding_excess,0);
  // In this case we are not going to be the toplevel object holding memory
  if (embedding_excess) {
    embedding_buffer->buf=res->mem->buf;
    embedding_buffer->len=res->mem->len;
    embedding_buffer->allocated=true;
    // We reserve the first part of the buffer for the toplevel object
    free(res->mem);
    res->mem=mb_new(total_memory,embedding_buffer->buf+embedding_excess);
  }
#ifdef PRINT_SIZES_AND_LOG
  printf("(wt_new): Compressed size is %ld B (toplevel=%ld,headers=%ld,bitmaps=%ld)\n",
        total_memory,total_memory_toplevel,total_memory_headers,total_memory_bitmaps);
  fflush(stdout);
#endif
  // We distribute memory
#define SLICE_MEMORY \
  void* base_toplevel=res->mem->buf; \
  void* base_headers=base_toplevel+total_memory_toplevel; \
  void* base_bitmaps=base_headers+total_memory_headers
  //
  SLICE_MEMORY;
  const void* old_base_headers=base_headers,* old_base_bitmaps=base_bitmaps;
  // Global header
  *(sa_t*)base_toplevel=res->n;
  base_toplevel+=sizeof(sa_t);
  *(sa_t*)base_toplevel=res->alpha;
  base_toplevel+=sizeof(sa_t);
  memcpy(base_toplevel,res->c,sizeof(res->c));
  base_toplevel+=sizeof(res->c);
  *(sa_t*)base_toplevel=res->red_isa_0;
  base_toplevel+=sizeof(sa_t);
  *(sa_t*)base_toplevel=res->char_red_isa_0;
  base_toplevel+=sizeof(sa_t);
  *(sa_t*)base_toplevel=node_num;
  // Bitmaps
  for (i=0;i<node_num;++i) {
    node_aux=node_list+i;
    node_aux_node=node_aux->node;
    const sa_t tmp=(((sa_t)node_aux_node->min_char)<<16)|((sa_t)node_aux_node->max_char);
    *(sa_t*)base_headers=tmp;
    base_headers+=sizeof(sa_t);
    if (!node_aux_node->is_terminal) {
      const size_t size=bb_size(node_aux->bitmap_len,node_aux->bitmap,superblock_size,block_size);
      // We build the compressed bitmap...
      node_aux_node->bitmap=
        bb_new(node_aux->bitmap_len,node_aux->bitmap,superblock_size,block_size,mb_new(size,base_bitmaps));
      base_bitmaps+=size;
      // ...check everything went fine...
      sa_t exp_rank=0;
      for (j=0;j<node_aux->bitmap_len;++j) {
        bool bit_orig=node_aux->bitmap[j/64]&(1ULL<<(63-(j%64))),bit_cmpr;
        sa_t rank;
        bb_rank_bit(node_aux_node->bitmap,j,&rank,&bit_cmpr);
        fflush(stdout);
        assert((bit_cmpr==bit_orig&&rank==exp_rank)||(
               printf("ERROR: Node=%d, len="SA_T_FMT" | Pos %d: orig_bit=%s bit=%s exp_rank="SA_T_FMT
                      " rank="SA_T_FMT"\n",
                      i,node_aux->bitmap_len,j,bit_orig?"1":"0",bit_cmpr?"1":"0",exp_rank,rank)&&
               fflush(stdout)));
        if (bit_orig)
          ++exp_rank;
      }
      // ...and delete the uncompressed one
      free(node_aux->bitmap);
      // We store the compressed length of the bitmap, otherwise it would be impossible to reconstruct the tree
      *(sa_t*)base_headers=size;
      base_headers+=sizeof(sa_t);
    } else {
      node_aux_node->bitmap=0;
      // We store the length anyway to keep memory aligned
      *(sa_t*)base_headers=0;
      base_headers+=sizeof(sa_t);
    }
#ifdef DEBUG_WT_NEW
    printf("Allocating node (chars=[%d-%d], uncompressed length="SA_T_FMT", compressed size=%ld B)\n",
           node_aux_node->min_char,node_aux_node->max_char,node_aux->bitmap_len,
           node_aux_node->bitmap->mem->len);
    fflush(stdout);
#endif
  }
  assert(base_headers-old_base_headers==total_memory_headers&&
         base_bitmaps-old_base_bitmaps==total_memory_bitmaps&&
         base_bitmaps-res->mem->buf==res->mem->len);
  //
  free(node_list);
  free(char_to_nodes);
  free(node_nums);
  return res;
}

void wt_write(WT* const wt,const char* filename) {
  if (!wt->mem->allocated)
    errx(1,"(wt_write) This function should never be called on an embedded object");
  // We write the data to the specified file
  mb_write(wt->mem,filename);
  return;
}

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
  assert(base==res->mem->buf+total_memory_toplevel);
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
  assert(num_nodes==0);
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
  assert(base_bitmaps-res->mem->buf==res->mem->len&&
         (base-res->mem->buf)+total_memory_bitmaps==res->mem->len);
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

// Computes the occurrencies of "c" in WT[0..i]
sa_t wt_rank(WT* const wt,ch_t c,sa_t i) {
  if (i>=wt->n)
    errx(1,"(wt_rank_encoded) Requested index "SA_T_FMT" is out of range",i);
#ifdef DEBUG_WT_RANK
  printf(" "SA_T_FMT,i);
  fflush(stdout);
#endif
  sa_t res=i+1;
  WTNode* node=wt->root;
loop:
  if (node->is_terminal)
    return res;
  sa_t rank;
  bool tmp;
  bb_rank_bit(node->bitmap,res,&rank,&tmp);
  if (c==node->min_char) {
#ifdef DEBUG_WT_RANK
    printf(" ==> (<) r="SA_T_FMT,rank);
    fflush(stdout);
#endif
    res-=rank;
#ifdef DEBUG_WT_RANK
    printf(" -> "SA_T_FMT,res);
    fflush(stdout);
#endif
    node=node->left;
  } else {
#ifdef DEBUG_WT_RANK
    printf(" ==> (>) r="SA_T_FMT,rank);
    fflush(stdout);
#endif
    res=rank;
#ifdef DEBUG_WT_RANK
    printf(" -> "SA_T_FMT,res);
    fflush(stdout);
#endif
    node=node->right;
  }
  goto loop;
}

// Decodes WT[i]
ch_t wt_char(WT* const wt,sa_t i) { /*DEC*/
  if (i>=wt->n)
    errx(1,"(wt_char) Requested index "SA_T_FMT" is out of range",i);
  sa_t where=i;
  WTNode* node=wt->root;
#ifdef DEBUG_WT_CHAR
  printf("wt_char("SA_T_FMT")",where);
  fflush(stdout);
#endif
  while (!node->is_terminal) {
    sa_t rank;
    bool bit;
    bb_rank_bit(node->bitmap,where,&rank,&bit);
    if (bit) {
      where=rank;
#ifdef DEBUG_WT_CHAR
      printf(" ==> (>) "SA_T_FMT" -> "SA_T_FMT,rank,where);
      fflush(stdout);
#endif
      node=node->right;
    } else {
      where-=rank;
#ifdef DEBUG_WT_CHAR
      printf(" ==> (<) "SA_T_FMT" -> "SA_T_FMT,rank,where);
      fflush(stdout);
#endif
      node=node->left;
    }
  }
#ifdef DEBUG_WT_CHAR
  printf(" ==> %d\n",node->min_char);
  fflush(stdout);
#endif
  return node->min_char;
}

sa_t bwt_LF(WT* const wt,sa_t i) { /*DEC*/
  if (i>=wt->n)
    errx(1,"(bwt_LF) Requested index "SA_T_FMT" is out of range",i);
  sa_t where=i;
  WTNode* node=wt->root;
#ifdef DEBUG_WT_LF
  printf("bwt_LF("SA_T_FMT")",where);
  fflush(stdout);
#endif
  while (!node->is_terminal) {
    sa_t rank;
    bool bit;
    bb_rank_bit(node->bitmap,where,&rank,&bit);
    if (bit) {
#ifdef DEBUG_WT_LF
      printf(" ==> (>) [lo=%d,hi=%d] ["SA_T_FMT"] "SA_T_FMT" -> "SA_T_FMT"->"SA_T_FMT" ["SA_T_FMT"]",
            node->min_char,node->max_char,(where&255ULL)>>6,rank,where,rank,(rank&255ULL)>>6);
      fflush(stdout);
#endif
      where=rank;
      node=node->right;
    } else {
#ifdef DEBUG_WT_LF
      printf(" ==> (<) [lo=%d,hi=%d] ["SA_T_FMT"] "SA_T_FMT" -> "SA_T_FMT"->"SA_T_FMT" ["SA_T_FMT"]",
            node->min_char,node->max_char,(where&255ULL)>>6,rank,where,where-rank,((where-rank)&255ULL)>>6);
      fflush(stdout);
#endif
      where-=rank;
      node=node->left;
    }
  }
#ifdef DEBUG_WT_LF
  printf(" ==> c=%d, C[%d]="SA_T_FMT", rank="SA_T_FMT", delta=%d, res="SA_T_FMT"\n",
          node->min_char,node->min_char,wt->c[node->min_char],where,
          (node->min_char==wt->char_red_isa_0&&i<wt->red_isa_0?1:0),
          where+wt->c[node->min_char]+(node->min_char==wt->char_red_isa_0&&i<wt->red_isa_0?1:0));
  fflush(stdout);
#endif
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

#include <err.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "utils.h"

uint64_t ceil_div(const uint64_t n,const uint64_t m) { return n/m+(n%m?1:0); } /*DEC*/

int ceil_log2(const uint64_t x) { /*DEC*/
  assert(x>0);
  int shift,res;
  for (shift=63;shift>-1;--shift)
    if ((x>>shift)&1ULL) {
      // This is bound to happen eventually
      res=shift;
      if (shift>0&&x<<(64-shift))
        ++res;
      break;    
    }
  return res;
}

// Optimised version of the previous one

const uint8_t ceil_log2_uint6_table[64] = { /*DEC*/
  0, 0, 1, 2, 2, 3, 3, 3,
  3, 4, 4, 4, 4, 4, 4, 4,
  4, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6
};

uint8_t ceil_log2_uint6(const uint8_t x) { /*DEC*/
  assert(x>0&&x<64);
  return ceil_log2_uint6_table[x];
}

#ifdef __SSE4_2__
  #include <nmmintrin.h>
  #define POPCOUNT64 _mm_popcnt_u64
#else
  #define POPCOUNT64 __builtin_popcountll
#endif

int popcount64(const uint64_t n) { return POPCOUNT64(n); } /*DEC*/

MB* mb_new(const size_t size,void* const embedding) { /*DEC*/
  MB* res=calloc(1,sizeof(MB));
  if (!res)
    err(1,"(mb_new) Could not allocate handler");
  res->len=size;
  if (embedding) {
    res->buf=embedding;
    res->allocated=false;
  } else {
    if (size) {
      res->buf=calloc(size,1);
      if (!res->buf)
        err(1,"(mb_new) Could not allocate buffer");
    }
    res->allocated=true;
  }
  return res;
}

#define MB_CHUNK_SIZE 1073741824ULL /* Needed for the ugly workaround :( */

void mb_write(const MB* const mem,const char* const fname) { /*DEC*/
  if (!mem->allocated)
    errx(1,"(mb_write) This function should never be called on an embedded buffer");
  // We just do a physical dump of the memory up to the declared/sliced boundary
  int fd=open(fname,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  if (fd==-1)
    err(1,"(mb_write) Could not open file '%s'",fname);
  // Workaround to bypass the mysterious 2^31 limit
  size_t rem_len=mem->len;
  int64_t written;
  while (rem_len>0) {
    written=write(fd,mem->buf+mem->len-rem_len,rem_len>MB_CHUNK_SIZE?MB_CHUNK_SIZE:rem_len);
    if (written==-1)
      err(1,"(mb_write) Could not write to file '%s'",fname);
    rem_len-=written;
  }
  //
  if (close(fd))
    err(1,"(mb_write) Could not close file '%s'",fname);
  return;
}

MB* mb_read(const char* const fname) { /*DEC*/
  MB* res=calloc(1,sizeof(MB));
  if (!res)
    err(1,"(mb_read) Could not allocate handler");
  res->allocated=true;
  struct stat statbuf;
  if (stat(fname,&statbuf)==-1)
    err(1,"(mb_read) Could not stat file '%s'",fname);
  int fd=open(fname,O_RDONLY,0);
  if (fd==-1)
    err(1,"(mb_read) Could not open file '%s'",fname);
  res->len=statbuf.st_size;
  if ((res->buf=malloc(res->len))) {
    // Workaround to bypass the mysterious 2^31 limit
    void* const buf=res->buf;
    size_t rem_len=res->len;
    int64_t b_read;
    while (rem_len>0) {
      b_read=read(fd,buf+res->len-rem_len,rem_len>MB_CHUNK_SIZE?MB_CHUNK_SIZE:rem_len);
      if (b_read==-1||(!b_read&&rem_len>0)) // 0 means EOF here
        err(1,"(mb_read) Could not read from file '%s'",fname);
      rem_len-=b_read;
    }
    if (close(fd))
      err(1,"(mb_read) Could not close file '%s'",fname);
    return res;
  } else
    err(1,"(mb_read) Could not allocate buffer");
}

void mb_delete(MB* const m) { /*DEC*/
  if (m->allocated)
    free(m->buf);
  free(m);
  return;  
}

#undef POPCOUNT64
#undef MB_CHUNK_SIZE

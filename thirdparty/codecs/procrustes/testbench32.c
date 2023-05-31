#include "testbench.h"
#include "CSA.h"
#include "utils.h"

int testbech_procrustes_encode_32(
  unsigned char *dest,
  unsigned long *destLen,
  unsigned char *src,
  unsigned long  srcLen,
  unsigned long  sampling_rate_sa,
  unsigned long  sampling_rate_lf,
  unsigned long  superblock_size,
  unsigned long  block_size
){
  
  int rc = -1;

  if(superblock_size == 0){
    superblock_size = ceil_log2(srcLen);
  }
  
  
  ch_t fmindex_decode_input_init(const sa_t n) {
    return src[n];
  }
  
  CSA *csa = csa_new(srcLen / sizeof(ch_t), fmindex_decode_input_init, sampling_rate_sa,
                     sampling_rate_lf, superblock_size, block_size, true, false);
  
  
  if (csa != NULL) {
    memcpy(dest, csa->mem->buf, csa->mem->len);
    *destLen = csa->mem->len;
    csa_delete(csa);
    rc = 0;
  }
  
  return rc;
  
}

int testbech_procrustes_decode_32(
  unsigned char *dest,
  unsigned long *destLen,
  unsigned char *src,
  unsigned long  srcLen
){
  
  int rc = -1;
  
  MB* buf=mb_new(srcLen,src);
  
  CSA* csa=csa_read(0,buf);
  
  MB* text=csa_decode(csa,0,csa->bwt->n,false);
  
  if (text != NULL) {
    memcpy(dest, text->buf, text->len);
    *destLen=text->len;
    mb_delete(text);
    csa_delete(csa);
    rc = 0;
  }
  
  return rc;
  
}








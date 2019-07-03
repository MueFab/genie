/** @file range.h
 *  @brief This file contains the interface to the range codec.
 *
 *  Note it is up to the calling code to ensure that no overruns on input and
 *  output buffers occur!
 *
 *  @author Jan Voges (voges)
 *  @bug No known bugs
 */

#ifndef RANGE_H_
#define RANGE_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned char * range_compress_o0(unsigned char *in,
                                  unsigned int  in_sz,
                                  unsigned int  *out_sz);
unsigned char * range_decompress_o0(unsigned char *in,
                                   //unsigned int   in_sz,
                                   unsigned int   *out_sz);
unsigned char * range_compress_o1(unsigned char *in,
                                  unsigned int  in_sz,
                                  unsigned int  *out_sz);
unsigned char * range_decompress_o1(unsigned char *in,
                                    //unsigned int  in_sz,
                                    unsigned int  *out_sz);

#ifdef __cplusplus
}
#endif

#endif // RANGE_H_


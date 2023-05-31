#ifndef __FMINDEX_TESTBENCH_H__
#define __FMINDEX_TESTBENCH_H__

#include "CSA.h"

int testbech_procrustes_encode_32(
				unsigned char *dest,
		        unsigned long *destLen,
		        unsigned char *src,
		        unsigned long  srcLen,
				unsigned long  sampling_rate_sa,
				unsigned long  sampling_rate_lf,
				unsigned long  superblock_size,
				unsigned long  block_size
		);

/*
int testbech_procrustes_encode_64(
				unsigned char *dest,
		        unsigned long *destLen,
		        unsigned char *src,
		        unsigned long  srcLen,
				unsigned long  sampling_rate_sa,
				unsigned long  sampling_rate_lf,
				unsigned long  superblock_size
		);
*/

int testbech_procrustes_decode_32(
				unsigned char *dest,
		        unsigned long *destLen,
		        unsigned char *src,
		        unsigned long  srcLen
		);

/*
int testbech_procrustes_decode_64(
				unsigned char *dest,
		        unsigned long *destLen,
		        unsigned char *src,
		        unsigned long  srcLen
		);
*/
#endif // __FMINDEX_TESTBENCH_H__






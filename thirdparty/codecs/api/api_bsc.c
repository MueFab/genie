#include <stdlib.h>
#include <string.h>
#include "libbsc.h"
#include "mpegg-codecs.h"

#define MPEGG_LIBBSC_FEATURE LIBBSC_FEATURE_FASTMODE

static int bsc_init_done = 0;

/**
* Compress a memory block.
* @param input                              - the input memory block of n bytes.
* @param output                             - the output memory block of n + LIBBSC_HEADER_SIZE bytes.
* @param n                                  - the length of the input memory block.
* @param lzpHashSize[0, 10..28]             - the hash table size if LZP enabled, 0 otherwise.
* @param lzpMinLen[0, 4..255]               - the minimum match length if LZP enabled, 0 otherwise.
* @param blockSorter[ST3..ST8, BWT]         - the block sorting algorithm.
* @param coder[MTF or QLFC]                 - the entropy coding algorithm.
* @return the length of compressed memory block if no error occurred, error code otherwise.
*/
int mpegg_bsc_compress(
        unsigned char      **dest,
        size_t              *destLen,
        const unsigned char *src,
        size_t               srcLen,

        int lzpHashSize,
        int lzpMinLen,
        int blockSorter,
        int coder){

    if(!bsc_init_done){
        int rc = 0;

        if((rc = bsc_init(MPEGG_LIBBSC_FEATURE)) != 0){
            return rc;
        }

        bsc_init_done = 1;
    }

    int ec = 0;
    int len = 0;
    unsigned long int dstLenTot = (srcLen + 20) / 20 * 21 + (1U << 16U) + MPEGG_BSC_HEADER_SIZE; /* we allocate 105% of original size + 64KB for output buffer */

    unsigned char *tmpBuffer = (unsigned char *) malloc(dstLenTot * sizeof(unsigned char));
    if (tmpBuffer == NULL) {
        ec = 1; goto error;
    }

    len = bsc_compress(src, tmpBuffer, srcLen, lzpHashSize, lzpMinLen, blockSorter, coder, MPEGG_LIBBSC_FEATURE);
    if(len <= 0){
        ec = 1; goto error;
    }

    error:

    if(ec != 0){
        if(tmpBuffer){
            free(tmpBuffer);
        }
        tmpBuffer = NULL;
        len = 0;
    }

    *dest = tmpBuffer;
    *destLen = len;

    return ec;
}

/**
* Decompress a memory block.
* Note : You should call bsc_block_info function to determinate the sizes of input and output memory blocks.
* @param input                              - the input memory block of inputSize bytes.
* @param inputSize                          - the length of the input memory block.
* @param output                             - the output memory block of outputSize bytes.
* @param outputSize                         - the length of the output memory block.
* @return LIBBSC_NO_ERROR if no error occurred, error code otherwise.
*/
int  mpegg_bsc_decompress(
        unsigned char      **dest,
        size_t              *destLen,
        const unsigned char *src,
        size_t               srcLen
        ){
    int ec = 0;

    if(!bsc_init_done){
        if((ec = bsc_init(MPEGG_LIBBSC_FEATURE)) != 0){
            return ec;
        }

        bsc_init_done = 1;
    }

    int blockSize = 0, dataSize = 0;
    unsigned char *tmpBuffer = NULL;

    if((ec = bsc_block_info(src, srcLen, &blockSize, &dataSize, MPEGG_LIBBSC_FEATURE)) != 0){
        ec = 1; goto error;
    }

    tmpBuffer = (unsigned char *) malloc(dataSize * sizeof(unsigned char));
    if (tmpBuffer == NULL) {
        ec = 1; goto error;
    }

    if((ec = bsc_decompress(src, srcLen, tmpBuffer, dataSize, MPEGG_LIBBSC_FEATURE)) != 0){
        ec = 1; goto error;
    }


    error:

    if(ec != 0){
        if(tmpBuffer){
            free(tmpBuffer);
        }
        tmpBuffer = NULL;
        dataSize = 0;
    }

    *dest = tmpBuffer;
    *destLen = dataSize;


    return ec;
}
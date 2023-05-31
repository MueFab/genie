#include <stdlib.h>
#include <string.h>
#include "zstd.h"
#include "mpegg-codecs.h"

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
int mpegg_zstd_compress(
        unsigned char      **dest,
        size_t              *destLen,
        const unsigned char *src,
        size_t               srcLen,
        int                  compressionLevel){

    int ec = 0;
    int len = 0;
    unsigned long int dstLenTot = (srcLen + 20) / 20 * 21 + (1U << 16U); /* we allocate 105% of original size + 64KB for output buffer */
    unsigned long int minOutSize = ZSTD_compressBound(srcLen) + 1;

    dstLenTot = dstLenTot >= minOutSize ? dstLenTot : minOutSize;

    unsigned char *tmpBuffer = (unsigned char *) malloc(dstLenTot * sizeof(unsigned char));
    if (tmpBuffer == NULL) {
        ec = 1; goto error;
    }

    len = ZSTD_compress(tmpBuffer, dstLenTot, src, srcLen, compressionLevel);
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


int  mpegg_zstd_decompress(
        unsigned char      **dest,
        size_t              *destLen,
        const unsigned char *src,
        size_t               srcLen
){
    int ec = 0;

    int decSize = 0, dataSize = 0;
    unsigned char *tmpBuffer = NULL;

    dataSize = ZSTD_getFrameContentSize(src, srcLen);
    if(dataSize <= 0){
        ec = 1; goto error;
    }

    tmpBuffer = (unsigned char *) malloc(dataSize * sizeof(unsigned char));
    if (tmpBuffer == NULL) {
        ec = 1; goto error;
    }

    decSize = ZSTD_decompress(tmpBuffer, dataSize, src, srcLen);
    if(decSize != dataSize){
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

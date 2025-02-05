#include <stdlib.h>
#include <string.h>
#include "LzmaLib.h"
#include "mpegg_utils.h"
#include "mpegg-codecs.h"

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_lzma_compress_default(
    unsigned char      **dest,
    size_t              *destLen,
    const unsigned char *src,
    size_t               srcLen
){
    return mpegg_lzma_compress(
        dest,
        destLen,
        src,
        srcLen,
        MPEGG_LZMA_DEFAULT_LEVEL,
        MPEGG_LZMA_DEFAULT_DIC_SIZE,
        MPEGG_LZMA_DEFAULT_LC,
        MPEGG_LZMA_DEFAULT_LP,
        MPEGG_LZMA_DEFAULT_PB,
        MPEGG_LZMA_DEFAULT_FB,
        MPEGG_LZMA_DEFAULT_THREADS
    );
};

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_lzma_compress(
        unsigned char      **dest,
        size_t              *destLen,
        const unsigned char *src,
        size_t               srcLen,
        int      level,     /* 0 <= level <= 9, default = 5 */
        unsigned dictSize,  /* default = (1 << 24) */
        int      lc,        /* 0 <= lc <= 8, default = 3  */
        int      lp,        /* 0 <= lp <= 4, default = 0  */
        int      pb,        /* 0 <= pb <= 4, default = 2  */
        int      fb,        /* 5 <= fb <= 273, default = 32 */
        int      numThreads /* 1 or 2, default = 2 */
){
    size_t propsSize = LZMA_PROPS_SIZE;
    size_t const headerSize = propsSize                                    /* signal propsSize */
                                         + MPEGG_LZMA_DATA_SIZE_BYTES;     /* signal srcLen */
    size_t payloadSize = (srcLen + 20) / 20 * 21 + (1U << 16U); /* we allocate 105% of original size + 64KB for output buffer */
    size_t const dstLenTot = headerSize + payloadSize + 1;      /* +1 for null termination */

    unsigned char *propsPtr = NULL;
    unsigned char *dataSzPtr = NULL;
    unsigned char *payloadPtr = NULL;
    unsigned char *tmpBuffer = NULL;
    unsigned char *trmBuffer = NULL;
    size_t trmSize = 0;

    int ec = 0;

    if (dest == NULL || src == NULL  || destLen == NULL || srcLen <= 0) {
        ec = 1; goto error;
    }

    tmpBuffer = (unsigned char *) malloc(dstLenTot * sizeof(unsigned char));
    if (tmpBuffer == NULL) {
        ec = 1; goto error;
    }

    propsPtr = tmpBuffer;
    dataSzPtr = tmpBuffer + LZMA_PROPS_SIZE;
    payloadPtr = tmpBuffer + headerSize;

    if (LzmaCompress(payloadPtr,
                     &payloadSize,
                     src,
                     srcLen,
                     propsPtr,
                     &propsSize,
                     level,
                     dictSize,
                     lc,
                     lp,
                     pb,
                     fb,
                     numThreads
    ) != SZ_OK) {
        ec = 1; goto error;
    }

    if ((ec = mpegg_write_long_to_Buffer(dataSzPtr,
                                          srcLen,
                                          MPEGG_LZMA_DATA_SIZE_BYTES)
    ) != 0) goto error;


    // create the trimmed version of the destination buffer
    trmSize = headerSize + payloadSize;
    trmBuffer = malloc((trmSize + 1) * sizeof(*tmpBuffer));
    if (trmBuffer == NULL) {
        ec = 1; goto error;
    }

    // finalize trmBuffer: set as null-terminated
    memcpy(trmBuffer, tmpBuffer, trmSize * sizeof(*tmpBuffer));
    trmBuffer[trmSize] = '\0';

    // set vars to be returned
    *dest = trmBuffer;
    *destLen = trmSize;


    error:

    if (tmpBuffer != NULL) {
        free(tmpBuffer);
    }

    if (ec != 0) {
        if (dest != NULL) {
            *dest = NULL;
        }

        if (destLen != NULL) {
            *destLen = 0;
        }
    }

    return ec;
}

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_lzma_decompress(
        unsigned char      **dest,
        size_t              *destLen,
        const unsigned char *src,
        size_t               srcLen){

    int ec = 0;

    unsigned long int propsSize  = MPEGG_LZMA_PROPS_SIZE;
    unsigned long int headerSize = MPEGG_LZMA_DATA_SIZE_BYTES /* parse and return in *dstLen */
                                   + propsSize; /* parse propsSize */

    unsigned char *dstPtr   = NULL;
    const unsigned char *srcPtr   = NULL;
    const unsigned char *propsPtr = NULL;


    if (dest == NULL || src == NULL  || destLen == NULL || srcLen <= 0) {
        ec = 1; goto error;
    }

    size_t tmpInt = 0;

    if((ec = mpegg_read_long_from_buffer(src + MPEGG_LZMA_PROPS_SIZE,
                                          &tmpInt,
                                          MPEGG_LZMA_DATA_SIZE_BYTES)
       ) != 0) goto error;

    if(tmpInt <= 0) {
        ec = 1; goto error;
    }

    size_t dstLenTot = tmpInt + 1; /* +1 for null terminator */
    size_t recLen    = tmpInt;
    size_t srcSz     = 0;

    dstPtr = (unsigned char *) malloc(dstLenTot * sizeof(unsigned char));
    if(dstPtr == NULL) {
        ec = 1; goto error;
    }

    propsPtr = src;
    srcPtr   = src + headerSize;
    srcSz    = srcLen - headerSize;

    if(LzmaUncompress(dstPtr,
                      &recLen,
                      srcPtr,
                      &srcSz,
                      propsPtr,
                      LZMA_PROPS_SIZE
    ) != SZ_OK) {
        ec = 1; goto error;
    }

    // finalize payload_: set as null-terminated
    *(dstPtr + recLen) = '\0';

    // set vars to be returned
    *dest = dstPtr;
    *destLen = recLen;

    error:

    if(ec != 0){

        if(dstPtr != NULL){
            free(dstPtr);
        }

        if(dest != NULL){
            *dest = NULL;
        }

        if(destLen != NULL){
            *destLen = 0;
        }
    }

    return ec;
}


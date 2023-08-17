#ifndef _MPEGG_CODECS_H
#define _MPEGG_CODECS_H

#include <stdbool.h>
#include <stddef.h>

/* ================================================================================================================  */
/* BSC                                                                                                               */
/* ================================================================================================================  */

#define MPEGG_BSC_BLOCKSORTER_NONE        0
#define MPEGG_BSC_BLOCKSORTER_BWT         1
#define MPEGG_BSC_CODER_NONE              0
#define MPEGG_BSC_CODER_QLFC_STATIC       1
#define MPEGG_BSC_CODER_QLFC_ADAPTIVE     2
#define MPEGG_BSC_CODER_QLFC_FAST         3
#define MPEGG_BSC_DEFAULT_LZPHASHSIZE     16
#define MPEGG_BSC_DEFAULT_LZPMINLEN       128
#define MPEGG_BSC_DEFAULT_BLOCKSORTER     MPEGG_BSC_BLOCKSORTER_BWT
#define MPEGG_BSC_DEFAULT_CODER           MPEGG_BSC_CODER_QLFC_STATIC
#define MPEGG_BSC_HEADER_SIZE             28

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compress a memory block.
 * @param input                              - the input memory block of n bytes.
 * @param output                             - the output memory block of n + MPEGG_BSC_HEADER_SIZE bytes.
 * @param n                                  - the length of the input memory block.
 * @param lzpHashSize[0, 10..28]             - the hash table size if LZP enabled, 0 otherwise.
 * @param lzpMinLen[0, 4..255]               - the minimum match length if LZP enabled, 0 otherwise.
 * @param blockSorter[ST3..ST8, BWT]         - the block sorting algorithm.
 * @param coder[MTF or QLFC]                 - the entropy coding algorithm.
 * @return the length of compressed memory block if no error occurred, error code otherwise.
 */
int mpegg_bsc_compress(unsigned char **dest, size_t *destLen, const unsigned char *src, size_t srcLen, int lzpHashSize,
                       int lzpMinLen, int blockSorter, int coder);

/**
 * Decompress a memory block.
 * Note : You should call bsc_block_info function to determinate the sizes of input and output memory blocks.
 * @param input                              - the input memory block of inputSize bytes.
 * @param inputSize                          - the length of the input memory block.
 * @param output                             - the output memory block of outputSize bytes.
 * @param outputSize                         - the length of the output memory block.
 * @param features                           - the set of additional features.
 * @return MPEGG_BSC_NO_ERROR if no error occurred, error code otherwise.
 */
int mpegg_bsc_decompress(unsigned char **dest, size_t *destLen, const unsigned char *src, size_t srcLen);

/* ================================================================================================================  */
/* LZMA                                                                                                              */
/* ================================================================================================================  */

#define MPEGG_LZMA_PROPS_SIZE 5
#define MPEGG_LZMA_DATA_SIZE_BYTES 8 /* 64-bits */
#define MPEGG_LZMA_DEFAULT_LEVEL 5
#define MPEGG_LZMA_DEFAULT_DIC_SIZE ((1U << 24U))
#define MPEGG_LZMA_DEFAULT_LC 3
#define MPEGG_LZMA_DEFAULT_LP 0
#define MPEGG_LZMA_DEFAULT_PB 2
#define MPEGG_LZMA_DEFAULT_FB 32
#define MPEGG_LZMA_DEFAULT_THREADS 2

int mpegg_lzma_compress(unsigned char **dest, size_t *destLen, const unsigned char *src, size_t srcLen,
                        int level,         /* 0 <= level <= 9, default = 5 */
                        unsigned dictSize, /* default = (1 << 24) */
                        int lc,            /* 0 <= lc <= 8, default = 3  */
                        int lp,            /* 0 <= lp <= 4, default = 0  */
                        int pb,            /* 0 <= pb <= 4, default = 2  */
                        int fb,            /* 5 <= fb <= 273, default = 32 */
                        int numThreads     /* 1 or 2, default = 2 */
);

int mpegg_lzma_decompress(unsigned char **dest, size_t *destLen, const unsigned char *src, size_t srcLen);

/* ================================================================================================================  */
/* ZSTD                                                                                                              */
/* ================================================================================================================  */

int mpegg_zstd_compress(unsigned char **dest, size_t *destLen, const unsigned char *src, size_t srcLen,
                        int compressionLevel);

int mpegg_zstd_decompress(unsigned char **dest, size_t *destLen, const unsigned char *src, size_t srcLen);

/* ================================================================================================================  */
/* JBIG                                                                                                              */
/* ================================================================================================================  */

int mpegg_jbig_compress(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char  *src,
    size_t                scr_len,
    unsigned long nrows,
    unsigned long ncols,
    unsigned long num_lines_per_stripe, // Valid range: 1 to 2^32-1
    bool deterministic_pred,
    bool typical_pred,
    bool diff_layer_typical_pred,
//    bool var_img_height,
    bool two_line_template
);

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_jbig_decompress_default(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               src_len,
    unsigned long       *nrows,
    unsigned long       *ncols);

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_jbig_decompress(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               src_len,
    unsigned long *nrows,
    unsigned long *ncols,
    size_t buf_len,
    size_t xmax
);

// ---------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // _MPEGG_CODECS_H
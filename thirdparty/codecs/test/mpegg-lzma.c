#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test-utils.h"
#include "mpegg-codecs.h"


void printUsage(FILE *out) {
    fprintf(out, "<input-file>\n");
}

int main(int argc, char **argv) {

    int rc = 0;
    int ret;
    unsigned char *inputBuffer        = NULL;
    unsigned char *compressedBuffer   = NULL;
    unsigned char *decompressedBuffer = NULL;

    size_t inputSize   = 0;
    size_t tmpBuffSize = 0;
    size_t compSize    = 0;

    if (argc < 2) {
        printUsage(stderr);
        rc = 1; goto error;
    }


    if((inputBuffer = load_binary_file(argv[1], &inputSize)) == NULL){
        rc = 1; goto error;
    }


    ret = mpegg_lzma_compress(
            &compressedBuffer,
            &compSize,
            inputBuffer,
            inputSize,
            MPEGG_LZMA_DEFAULT_LEVEL,
            MPEGG_LZMA_DEFAULT_DIC_SIZE,
            MPEGG_LZMA_DEFAULT_LC,
            MPEGG_LZMA_DEFAULT_LP,
            MPEGG_LZMA_DEFAULT_PB,
            MPEGG_LZMA_DEFAULT_FB,
            MPEGG_LZMA_DEFAULT_THREADS);

    if(ret != 0){
        fprintf(stderr, "compression error...\n");
        rc = 1; goto error;
    }

    fprintf(stdout, "raw       : %lu\n", inputSize);
    fprintf(stdout, "compressed: %lu\n", compSize);


    ret = mpegg_lzma_decompress(&decompressedBuffer, &tmpBuffSize, compressedBuffer, compSize);
    if(ret != 0){
        fprintf(stderr, "decompression error...\n");
        rc = 1; goto error;
    }

    ret = memcmp(decompressedBuffer, inputBuffer, inputSize);
    if(ret != 0){
        fprintf(stderr, "comparison error...\n");
        rc = 1; goto error;
    }

error:

    if(inputBuffer) free(inputBuffer);
    if(compressedBuffer) free(compressedBuffer);
    if(decompressedBuffer) free(decompressedBuffer);

    return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}



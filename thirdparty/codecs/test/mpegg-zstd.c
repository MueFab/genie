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
    size_t compBuffSize = 0;
    size_t decBuffSize  = 0;

    if (argc < 2) {
        printUsage(stderr);
        rc = 1; goto error;
    }


    if((inputBuffer = load_binary_file(argv[1], &inputSize)) == NULL){
        rc = 1; goto error;
    }

    ret = mpegg_zstd_compress(
            &compressedBuffer,
            &compBuffSize,
            inputBuffer,
            inputSize,
            4);

    if(ret != 0){
        fprintf(stderr, "compression error...\n");
        rc = 1; goto error;
    }

    ret = mpegg_zstd_decompress(&decompressedBuffer, &decBuffSize, compressedBuffer, compBuffSize);
    if(ret != 0){
        fprintf(stderr, "decompression error...\n");
        rc = 1; goto error;
    }

    fprintf(stdout, "raw         : %zu\n", inputSize);
    fprintf(stdout, "compressed  : %zu\n", compBuffSize);
    fprintf(stdout, "decompressed: %zu\n", decBuffSize);

    if(inputSize != decBuffSize){
        fprintf(stderr, "decompression error: different size...\n");
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



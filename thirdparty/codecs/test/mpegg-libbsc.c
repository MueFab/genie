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

    ret = mpegg_bsc_compress(
            &compressedBuffer,
            &compBuffSize,
            inputBuffer,
            inputSize,
            MPEGG_BSC_DEFAULT_LZPHASHSIZE,
            MPEGG_BSC_DEFAULT_LZPMINLEN,
            MPEGG_BSC_BLOCKSORTER_BWT,
            MPEGG_BSC_CODER_QLFC_STATIC);

    if(ret != 0){
        fprintf(stderr, "compression error...\n");
        rc = 1; goto error;
    }


    ret = mpegg_bsc_decompress(&decompressedBuffer, &decBuffSize, compressedBuffer, compBuffSize);
    if(ret != 0){
        fprintf(stderr, "decompression error...\n");
        rc = 1; goto error;
    }

    fprintf(stdout, "raw         : %lu\n", (unsigned long)inputSize);
    fprintf(stdout, "compressed  : %lu\n", (unsigned long)compBuffSize);
    fprintf(stdout, "decompressed: %lu\n", (unsigned long)decBuffSize);

    if(inputSize != decBuffSize){
        fprintf(stderr, "decompression error: different size...\n");
        rc = 1; goto error;
    }

    ret = memcmp(decompressedBuffer, inputBuffer, inputSize);
    if(ret != 0){
        fprintf(stderr, "comparison error...\n");
        rc = 1; goto error;
    }
    FILE *outP;

    char outputfile[] = "Test.bin";
    outP = fopen(outputfile, "wb");
    if (!outP) {
        printf("failed to open outputfile\n");
    }
    else {
        if (!fwrite(compressedBuffer, 1, compBuffSize, outP)) printf("write to outputfile failed\n");
        fclose(outP);
    }
error:

    if(inputBuffer) free(inputBuffer);
    if(compressedBuffer) free(compressedBuffer);
    if(decompressedBuffer) free(decompressedBuffer);



    return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}



#ifndef _MPEGG_TEST_UTILS_H
#define _MPEGG_TEST_UTILS_H


#include <stdio.h>
#include <stdlib.h>

unsigned char * load_binary_file(const char *file_path, size_t *size){
    if(file_path == NULL || size == NULL){
        fprintf(stderr, "bad parameters\n");
        return NULL;
    }

    int rc = 0;
    FILE *input = NULL;
    unsigned char *buffer = NULL;
    *size = 0;

    if((input = fopen(file_path, "rb")) == NULL){
        fprintf(stderr, "file cannot be opened\n");
        rc = 1; goto error;
    }

    // get file size
    fseek(input, 0, SEEK_END);
    *size = ftell(input);
    rewind(input);

    // allocate memory for buffer
    buffer = (unsigned char *)malloc(sizeof(*buffer) * (*size));
    if(buffer == NULL) {
        fprintf(stderr, "allocation failed\n");
        rc = 1; goto error;
    }

    // read file to buffer
    if(fread(buffer,(*size),1, input) != 1){
        fprintf(stderr, "file reading error\n");
        rc = 1; goto error;
    }

    error:
    if(rc != 0){
        if(buffer) free(buffer);

        *size  = 0;
        buffer = NULL;
    }

    if(input) fclose(input);

    return buffer;
}


#endif // _MPEGG_TEST_UTILS_H
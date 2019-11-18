//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../Boxes.h"

ByteArray* initByteArray() {
    ByteArray* byteArray = (ByteArray*)malloc(sizeof(ByteArray));
    byteArray->byte_array = NULL;
    byteArray->allocated_size = 0;
    byteArray->size = 0;
    return byteArray;
}

ByteArray* initByteArrayCopying(ByteArray* byteArray) {
    ByteArray* byteArrayResult = (ByteArray*)malloc(sizeof(ByteArray));
    if (byteArrayResult == NULL) {
        fprintf(stderr, "failed to create byte array");
        return NULL;
    }
    byteArrayResult->byte_array = (Byte*)malloc(byteArray->size * sizeof(Byte));
    if (byteArrayResult->byte_array == NULL) {
        fprintf(stderr, "failed to create data array in byte array");
        free(byteArrayResult);
        return NULL;
    }
    byteArrayResult->allocated_size = byteArray->size;
    memcpy(byteArrayResult->byte_array, byteArray->byte_array, byteArray->size);
    byteArrayResult->size = byteArray->size;
    return byteArrayResult;
}

ByteArray* initByteArrayWithSize(uint64_t size) {
    ByteArray* byteArray = (ByteArray*)malloc(sizeof(ByteArray));
    byteArray->byte_array = (Byte*)malloc(size * sizeof(Byte));
    if (byteArray->byte_array != NULL) {
        byteArray->allocated_size = size;
    } else {
        byteArray->allocated_size = 0;
    }
    byteArray->size = 0;
    return byteArray;
}

ByteArray* initByteArrayPassData(uint64_t size, Byte* data) {
    ByteArray* byteArray = (ByteArray*)malloc(sizeof(ByteArray));
    if (byteArray == NULL) {
        return NULL;
    }
    byteArray->byte_array = data;
    byteArray->size = size;
    byteArray->allocated_size = size;
    return byteArray;
}

void freeByteArray(ByteArray* byteArray) {
    free(byteArray->byte_array);
    free(byteArray);
}

bool writeByteArray(ByteArray* byteArray, FILE* outputFile) {
    size_t written_size = fwrite(byteArray->byte_array, sizeof(Byte), byteArray->size, outputFile);
    return written_size == byteArray->size;
}

uint64_t getSizeByteArray(ByteArray* byteArray) { return byteArray->size; }

bool copyBytesSource(ByteArray* byteArray, Byte* source, uint64_t source_size) {
    if (byteArray->allocated_size < source_size) {
        return false;
    }
    memcpy(byteArray->byte_array, source, source_size);
    byteArray->size = source_size;
    return true;
}

ByteArray* cloneByteArray(ByteArray* sourceByteArray) {
    ByteArray* newByteArray = initByteArrayWithSize(sourceByteArray->size);
    if (newByteArray == NULL) {
        return NULL;
    }
    memcpy(newByteArray->byte_array, sourceByteArray->byte_array, sourceByteArray->size);
    newByteArray->size = sourceByteArray->size;
    return newByteArray;
}
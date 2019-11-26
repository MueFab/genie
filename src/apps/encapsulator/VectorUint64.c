//
// Created by gencom on 17/11/17.
//

#include "vectorUint64.h"

VectorUint64* initVectorUint64() {
    VectorUint64* vectorUint64 = (VectorUint64*)malloc(sizeof(VectorUint64));
    vectorUint64->allocated = 0;
    vectorUint64->size = 0;

    vectorUint64->vector = malloc(sizeof(uint64_t) * 20);
    if (vectorUint64->vector != NULL) {
        vectorUint64->allocated = 20;
    }

    return vectorUint64;
}

bool resizeVectorUint64(VectorUint64* vectorUint64, size_t newSize) {
    uint64_t* newVectorUint64 = realloc(vectorUint64->vector, sizeof(uint64_t) * newSize);
    if (newVectorUint64 != NULL) {
        vectorUint64->vector = newVectorUint64;
        vectorUint64->allocated = newSize;
        if (vectorUint64->allocated < vectorUint64->size) {
            vectorUint64->size = vectorUint64->allocated;
        }
        return true;
    }
    return false;
}

void freeVectorUint64(VectorUint64* vectorUint64) {
    free(vectorUint64->vector);
    free(vectorUint64);
}

size_t getSizeVectorUint64(VectorUint64* vectorUint64) { return vectorUint64->size; }

size_t getAllocatedSizeVectorUint64(VectorUint64* vectorUint64) { return vectorUint64->allocated; }

bool pushBackVectorUint64(VectorUint64* vectorUint64, uint64_t value) {
    if (getSizeVectorUint64(vectorUint64) < getAllocatedSizeVectorUint64(vectorUint64)) {
        vectorUint64->vector[getSizeVectorUint64(vectorUint64)] = value;
        vectorUint64->size++;
        return true;
    }
    if (!resizeVectorUint64(vectorUint64, getSizeVectorUint64(vectorUint64) + 20)) {
        return false;
    }
    vectorUint64->vector[getSizeVectorUint64(vectorUint64)] = value;
    vectorUint64->size++;
    return true;
}

uint64_t getValueVectorUint64(VectorUint64* vectorUint64, size_t position) {
    if (position >= getSizeVectorUint64(vectorUint64)) {
        return 0;
    }
    return vectorUint64->vector[position];
}
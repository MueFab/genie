//
// Created by gencom on 17/11/17.
//

#ifndef MPEGG_CAPSULATOR_VectorUint64INT_H
#define MPEGG_CAPSULATOR_VectorUint64INT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint64_t *vector;
    size_t allocated;
    size_t size;
} VectorUint64;

VectorUint64 *initVectorUint64();
bool resizeVectorUint64(VectorUint64 *vectorUint64, size_t newSize);
size_t getSizeVectorUint64(VectorUint64 *vectorUint64);
size_t getAllocatedSizeVectorUint64(VectorUint64 *vectorUint64);
void freeVectorUint64(VectorUint64 *vectorUint64);
bool pushBackVectorUint64(VectorUint64 *vectorUint64, uint64_t value);
uint64_t getValueVectorUint64(VectorUint64 *vectorUint64, size_t position);

#endif  // MPEGG_CAPSULATOR_VectorUint64INT_H

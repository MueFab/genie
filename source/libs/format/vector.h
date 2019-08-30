//
// Created by gencom on 15/11/17.
//

#include <stdbool.h>
#include <stddef.h>

#ifndef MPEGG_CAPSULATOR_VECTOR_H
#define MPEGG_CAPSULATOR_VECTOR_H

typedef struct {
    void** vector;
    size_t allocated;
    size_t size;
} Vector;

Vector* initVector();
bool reallocateVector(Vector* vector, size_t newSize);
bool resizeVector(Vector* vector, size_t newSize);
size_t getSize(Vector* vector);
size_t getAllocatedSize(Vector* vector);
void freeVector(Vector* vector);
bool pushBack(Vector* vector, void* value);
void* getValue(Vector* vector, size_t position);
bool setValue(Vector* vector, size_t position, void* value);
Vector* shallowCopy(Vector* vector);

#endif  // MPEGG_CAPSULATOR_VECTOR_H

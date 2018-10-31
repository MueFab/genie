//
// Created by gencom on 15/11/17.
//



#ifndef MPEGG_CAPSULATOR_VECTOR_H
#define MPEGG_CAPSULATOR_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct{
    void** vector;
    size_t allocated;
    size_t size;
} Vector;

Vector* initVector();
bool reallocateVector(Vector* vector, size_t newSize);
bool resizeVector(Vector *vector, size_t newSize);
size_t getSize(Vector* vector);
size_t getAllocatedSize(Vector* vector);
void freeVector(Vector *vector);
bool pushBack(Vector* vector, void* value);
void* getValue(Vector* vector, size_t position);
bool setValue(Vector *vector, size_t position, void *value);
Vector* shallowCopy(Vector *vector);

#ifdef __cplusplus
}
#endif

#endif //MPEGG_CAPSULATOR_VECTOR_H


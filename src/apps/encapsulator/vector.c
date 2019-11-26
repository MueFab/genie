//
// Created by gencom on 15/11/17.
//

#include "vector.h"
#include <stdlib.h>

Vector* initVector() {
    Vector* vector = (Vector*)malloc(sizeof(Vector));
    vector->allocated = 0;
    vector->size = 0;

    vector->vector = malloc(sizeof(void*) * 20);
    if (vector->vector != NULL) {
        vector->allocated = 20;
    }

    return vector;
}

bool reallocateVector(Vector* vector, size_t newSize) {
    void** newVector = realloc(vector->vector, sizeof(void*) * newSize);
    if (newVector != NULL) {
        vector->vector = newVector;
        vector->allocated = newSize;
        if (vector->allocated < vector->size) {
            vector->size = vector->allocated;
        }
        return true;
    }
    return false;
}

bool resizeVector(Vector* vector, size_t newSize) {
    size_t old_size = vector->size;
    bool reallocateSuccessful = reallocateVector(vector, newSize);
    if (!reallocateSuccessful) {
        return false;
    }
    vector->size = newSize;
    if (old_size < newSize) {
        for (size_t element_i = old_size; element_i < newSize; element_i++) {
            vector->vector[element_i] = NULL;
        }
    }
    return true;
}

void freeVector(Vector* vector) {
    free(vector->vector);
    free(vector);
}

size_t getSize(Vector* vector) { return vector->size; }

size_t getAllocatedSize(Vector* vector) { return vector->allocated; }

bool pushBack(Vector* vector, void* value) {
    if (getSize(vector) < getAllocatedSize(vector)) {
        vector->vector[getSize(vector)] = value;
        vector->size++;
        return true;
    }
    if (!reallocateVector(vector, getSize(vector) + 20)) {
        return false;
    }
    vector->vector[getSize(vector)] = value;
    vector->size++;
    return true;
}

void* getValue(Vector* vector, size_t position) {
    if (position >= getSize(vector)) {
        return NULL;
    }
    return vector->vector[position];
}

bool setValue(Vector* vector, size_t position, void* value) {
    if (vector->allocated <= position) {
        if (!resizeVector(vector, position + 1)) {
            return false;
        }
    }
    vector->vector[position] = value;
    if (vector->size < position + 1) {
        vector->size = position + 1;
    }
    return true;
}

Vector* shallowCopy(Vector* vector) {
    Vector* newVector = calloc(1, sizeof(Vector));
    if (newVector == NULL) {
        return NULL;
    }
    newVector->vector = malloc(sizeof(void*) * vector->allocated);
    if (newVector->vector == NULL) {
        free(newVector);
        return NULL;
    }
    newVector->allocated = vector->allocated;
    newVector->size = vector->size;
    for (size_t element_i = 0; element_i < newVector->size; element_i++) {
        newVector->vector[element_i] = vector->vector[element_i];
    }
    return newVector;
}
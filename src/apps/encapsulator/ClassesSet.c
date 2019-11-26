//
// Created by gencom on 16/11/17.
//

#include "ClassesSet.h"
#include <stdbool.h>
#include <stdint.h>

ClassesSet initClassesSet() { return 0; }

void addClassToSet(ClassesSet* classesSet, uint8_t classId) {
    if (classId < 8) {
        uint8_t newVal = (uint8_t)1 << classId;
        *classesSet |= newVal;
    }
}

bool checkClass(ClassesSet classesSet, uint8_t classId) {
    if (classId < 8) {
        uint8_t newVal = (uint8_t)(1 << (classId));
        return classesSet & newVal;
    }
    return false;
}

uint8_t getSizeSet(ClassesSet classesSet) {
    uint8_t size = 0;
    for (uint8_t class_i = 0; class_i < 8; class_i++) {
        if (checkClass(classesSet, class_i)) {
            size++;
        }
    }
    return size;
}

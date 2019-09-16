//
// Created by gencom on 16/11/17.
//

#include <stdbool.h>
#include <stdint.h>

#ifndef MPEGG_CAPSULATOR_CLASSES_SET_H
#define MPEGG_CAPSULATOR_CLASSES_SET_H

typedef uint8_t ClassesSet;
ClassesSet initClassesSet();
void addClassToSet(ClassesSet* classesSet, uint8_t classId);
bool checkClass(ClassesSet classesSet, uint8_t classId);
uint8_t getSizeSet(ClassesSet classesSet);

#endif  // MPEGG_CAPSULATOR_CLASSES_SET_H

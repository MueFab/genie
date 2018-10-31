//
// Created by gencom on 16/11/17.
//


#ifndef MPEGG_CAPSULATOR_CLASSES_SET_H
#define MPEGG_CAPSULATOR_CLASSES_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t ClassesSet;
ClassesSet initClassesSet();
void addClassToSet(ClassesSet* classesSet, uint8_t classId);
bool checkClass(ClassesSet classesSet, uint8_t classId);
uint8_t getSizeSet(ClassesSet classesSet);

#ifdef __cplusplus
}
#endif

#endif //MPEGG_CAPSULATOR_CLASSES_SET_H

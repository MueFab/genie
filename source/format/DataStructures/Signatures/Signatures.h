//
// Created by gencom on 10/10/17.
//

#ifndef MPEGG_CAPSULATOR_SIGNATURES_H
#define MPEGG_CAPSULATOR_SIGNATURES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Signature.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    Signature* signature;
    int number_signatures;
    size_t allocated_signatures;
} Signatures;

Signatures* initSignatures(size_t size);
bool changeSizeSignatures(Signatures *signatures, size_t size);
Signature* getSignature(Signatures* signatures, int index);
void freeSignatures(Signatures* signatures);

#ifdef __cplusplus
}
#endif

#endif //MPEGG_CAPSULATOR_SIGNATURES_H

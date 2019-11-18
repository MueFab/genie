//
// Created by gencom on 10/10/17.
//

#ifndef MPEGG_CAPSULATOR_SIGNATURES_H
#define MPEGG_CAPSULATOR_SIGNATURES_H

#include <stdbool.h>
#include "Signature.h"

typedef struct {
    Signature** signature;
    uint32_t number_signatures;
    size_t allocated_signatures;
} Signatures;

Signatures* initSignatures(size_t size);
bool changeSizeSignatures(Signatures* signatures, size_t size);
Signature* getSignature(Signatures* signatures, size_t index);
void setSignature(Signatures* signatures, Signature* signature);
void freeSignatures(Signatures* signatures);

#endif  // MPEGG_CAPSULATOR_SIGNATURES_H

//
// Created by gencom on 10/10/17.
//

#include "Signatures.h"
#include <stdio.h>
#include <stdlib.h>

Signatures* initSignatures(size_t size) {
    Signatures* signatures = calloc(1, sizeof(Signatures));
    signatures->number_signatures = 0;
    signatures->signature = (Signature**)calloc(size, sizeof(Signature*));
    if (signatures->signature != NULL) {
        signatures->allocated_signatures = size;
        return signatures;
    }
    signatures->allocated_signatures = 0;
    signatures->signature = NULL;
    free(signatures);
    fprintf(stderr, "Error allocating signatures.\n");
    return NULL;
}

void setSignature(Signatures* signatures, Signature* signature) {
    signatures->signature[signatures->number_signatures] = signature;
    signatures->number_signatures++;
}

bool changeSizeSignatures(Signatures* signatures, size_t size) {
    Signature** newSignatureArray = (Signature**)realloc(signatures->signature, size * sizeof(Signature*));
    if (newSignatureArray == NULL) {
        free(signatures->signature);
        signatures->signature = NULL;
        signatures->allocated_signatures = 0;
        fprintf(stderr, "Error reallocating signatures.\n");
        return false;
    }
    signatures->signature = newSignatureArray;
    signatures->allocated_signatures = size;
    return true;
}

Signature* getSignature(Signatures* signatures, size_t index) {
    if (index >= signatures->allocated_signatures) {
        return NULL;
    }
    return signatures->signature[index];
}

void freeSignatures(Signatures* signatures) {
    for (size_t i = 0; i < signatures->allocated_signatures; i++) {
        freeSignature(signatures->signature[i]);
    }
    free(signatures->signature);
    free(signatures);
}

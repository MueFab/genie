//
// Created by gencom on 29/09/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_H
#define READ_SIGNATURES_SIGNATURE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t* symbols;
    uint32_t signatureLength;
    uint32_t allocatedLength;
} Signature;

int initSignature(Signature* signature);
int initSignatureWithData(Signature** signature, uint8_t* data, uint32_t signatureLength);
bool addSymbol(Signature* signature, uint8_t symbol);
void printSignature(Signature* signature);
void freeSignature(Signature* signature);

#endif  // READ_SIGNATURES_SIGNATURE_H

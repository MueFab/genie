//
// Created by gencom on 29/09/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_H
#define READ_SIGNATURES_SIGNATURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct{
    uint8_t* symbols;
    uint16_t signatureLength;
    uint16_t allocatedLength;
} Signature;

int initSignature(Signature* signature);
int initSignatureWithData(Signature* signature, uint8_t* data, uint16_t signatureLength);
int addSymbol(Signature* signature, uint8_t symbol);
void printSignature(Signature* signature);
void freeSignature(Signature* signature);

#ifdef __cplusplus
}
#endif

#endif //READ_SIGNATURES_SIGNATURE_H

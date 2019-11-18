//
// Created by gencom on 9/10/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_SIZE_COMPUTATION_H
#define READ_SIGNATURES_SIGNATURE_SIZE_COMPUTATION_H

#include <stdint.h>
#include "Signature.h"

typedef struct {
    uint32_t signatureLengthInSymbols;
    uint32_t currentSignatureSizeInSymbols;
    uint8_t integerSize;
    uint8_t symbolSize;
    uint8_t currentIntegerContentLength;
    uint64_t totalSizeInBits;
} SignatureSizeComputation;

void initSignatureSizeComputation(SignatureSizeComputation* signatureSizeComputation, uint32_t signatureLengthInSymbols,
                                  uint8_t integerSize, uint8_t symbolSize);

void simulateSignature(SignatureSizeComputation* signatureSizeComputation, Signature* signature);
void simulateSymbol(SignatureSizeComputation* signatureSizeComputation);
void simulateTerminator(SignatureSizeComputation* signatureSizeComputation);
uint64_t getTotalSizeInBits(SignatureSizeComputation* signatureSizeComputation);
uint64_t getTotalSizeInBytes(SignatureSizeComputation* signatureSizeComputation);

#endif  // READ_SIGNATURES_SIGNATURE_SIZE_COMPUTATION_H

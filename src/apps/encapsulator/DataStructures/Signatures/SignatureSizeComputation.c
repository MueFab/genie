//
// Created by gencom on 9/10/17.
//
#include "SignatureSizeComputation.h"
#include "../../utils.h"

void initSignatureSizeComputation(SignatureSizeComputation* signatureSizeComputation, uint32_t signatureLengthInSymbols,
                                  uint8_t integerSize, uint8_t symbolSize) {
    signatureSizeComputation->integerSize = integerSize;
    signatureSizeComputation->symbolSize = symbolSize;
    signatureSizeComputation->signatureLengthInSymbols = signatureLengthInSymbols;
    signatureSizeComputation->currentIntegerContentLength = 0;
    signatureSizeComputation->currentSignatureSizeInSymbols = 0;
    signatureSizeComputation->totalSizeInBits = 0;
}

void simulateSignature(SignatureSizeComputation* signatureSizeComputation, Signature* signature) {
    for (uint16_t symbol_i = 0; symbol_i < signature->signatureLength; symbol_i++) {
        simulateSymbol(signatureSizeComputation);
    }
    simulateTerminator(signatureSizeComputation);
}
void simulateSymbol(SignatureSizeComputation* signatureSizeComputation) {
    signatureSizeComputation->totalSizeInBits += signatureSizeComputation->symbolSize;
    signatureSizeComputation->currentIntegerContentLength += signatureSizeComputation->symbolSize;
    signatureSizeComputation->currentSignatureSizeInSymbols++;

    uint8_t sizeRemaining =
        signatureSizeComputation->integerSize - signatureSizeComputation->currentIntegerContentLength;

    if (signatureSizeComputation->signatureLengthInSymbols == signatureSizeComputation->currentSignatureSizeInSymbols ||
        sizeRemaining < signatureSizeComputation->symbolSize) {
        signatureSizeComputation->totalSizeInBits += sizeRemaining;

        signatureSizeComputation->currentIntegerContentLength = 0;

        if (signatureSizeComputation->signatureLengthInSymbols ==
            signatureSizeComputation->currentSignatureSizeInSymbols) {
            signatureSizeComputation->currentSignatureSizeInSymbols = 0;
        }
    }
}

void simulateTerminator(SignatureSizeComputation* signatureSizeComputation) {
    if (signatureSizeComputation->signatureLengthInSymbols != 0) return;
    signatureSizeComputation->totalSizeInBits += signatureSizeComputation->symbolSize;

    signatureSizeComputation->currentIntegerContentLength += signatureSizeComputation->symbolSize;
    uint8_t sizeRemaining =
        signatureSizeComputation->integerSize - signatureSizeComputation->currentIntegerContentLength;

    signatureSizeComputation->totalSizeInBits += sizeRemaining;
    signatureSizeComputation->currentIntegerContentLength = 0;
}

uint64_t getTotalSizeInBits(SignatureSizeComputation* signatureSizeComputation) {
    return signatureSizeComputation->totalSizeInBits;
}
uint64_t getTotalSizeInBytes(SignatureSizeComputation* signatureSizeComputation) {
    return bitsToBytes(signatureSizeComputation->totalSizeInBits);
}
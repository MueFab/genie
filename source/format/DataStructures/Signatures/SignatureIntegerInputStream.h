//
// Created by gencom on 29/09/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_INTEGER_STREAM_H
#define READ_SIGNATURES_SIGNATURE_INTEGER_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "SignatureInputInteger.h"
#include "Signature.h"
#include "../BitStreams/InputBitstream.h"

typedef struct {
    InputBitstream* inputFile;
    SignatureInputArbitraryLengthInteger signatureInteger;
    uint8_t symbolLength;
    uint8_t integerLength;
    uint8_t signatureSizeInSymbols;
    uint8_t currentSizeInSymbols;
    char* buffer;
} SignatureArbitraryLengthIntegerInputStream;

int initSignatureInputStream(SignatureArbitraryLengthIntegerInputStream* signatureArbitraryLengthIntegerInputStream,
                             InputBitstream* inputFile, uint8_t integerLength, uint8_t symbolLength,
                             uint8_t signatureSizeInSymbols);
int initSignatureInputStreamAndFirstInteger(SignatureArbitraryLengthIntegerInputStream* signatureArbitraryLengthIntegerInputStream,
                                            InputBitstream* inputFile, uint8_t integerLength, uint8_t symbolLength,
                                            uint8_t signatureSizeInSymbols, uint8_t* integerData);
int endOfStream(SignatureArbitraryLengthIntegerInputStream* signatureIntegerStream);
uint8_t getNextSymbol(SignatureArbitraryLengthIntegerInputStream* signatureIntegerStream);
Signature getSignatureFromInputStream(SignatureArbitraryLengthIntegerInputStream *signatureIntegerStream);
int getNewInteger(SignatureArbitraryLengthIntegerInputStream *stream);
void freeSignatureInputStream(SignatureArbitraryLengthIntegerInputStream* signatureArbitraryLengthIntegerInputStream);


#ifdef __cplusplus
}
#endif

#endif //READ_SIGNATURES_SIGNATURE_INTEGER_STREAM_H

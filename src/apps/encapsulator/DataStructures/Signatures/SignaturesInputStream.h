//
// Created by gencom on 29/09/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_INTEGER_STREAM_H
#define READ_SIGNATURES_SIGNATURE_INTEGER_STREAM_H

#include <stdio.h>
#include "../BitStreams/InputBitstream.h"
#include "Signature.h"
#include "SignatureInputInteger.h"
#include "Signatures.h"

typedef struct {
    InputBitstream* inputBitstream;
    bool firstSymbolProvided;
    uint64_t firstSymbol;
    uint8_t signatureSize;
    uint8_t u_signature_length;
    uint32_t numberSignatures;
    uint8_t alphabet;
    uint64_t currentInteger;
    uint8_t posCurrentInteger;
} SignaturesInputStream;

SignaturesInputStream* initSignatureInputStream(InputBitstream* inputBitstream, uint8_t signatureSize,
                                                uint8_t u_signature_length, uint32_t multipleSignatureBase,
                                                uint8_t alphabet);
Signatures* readSignatures(SignaturesInputStream* signaturesInputStream);
void freeSignatureInputStream(SignaturesInputStream* signaturesInputStream);

#endif  // READ_SIGNATURES_SIGNATURE_INTEGER_STREAM_H

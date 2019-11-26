//
// Created by gencom on 4/10/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_INTEGER_OUTPUT_STREAM_H
#define READ_SIGNATURES_SIGNATURE_INTEGER_OUTPUT_STREAM_H

#include <stdbool.h>
#include "../BitStreams/OutputBitstream.h"
#include "Signature.h"
#include "Signatures.h"

typedef struct {
    OutputBitstream* outputBitstream;

    uint8_t signatureSize;
    uint8_t u_signature_length;
    uint32_t multipleSignatureBase;
    uint8_t alphabet;
} SignaturesOutputStream;

SignaturesOutputStream* initSignaturesOutputStream(OutputBitstream* outputFile, uint32_t multipleSignatureBase,
                                                   uint8_t signatureSize, uint8_t u_signature_length, uint8_t alphabet);

int writeSignatures(SignaturesOutputStream* signaturesOutputStream, Signatures* signatures);
void freeSignaturesOutputStream(SignaturesOutputStream* signaturesOutputStream);

#endif  // READ_SIGNATURES_SIGNATURE_INTEGER_OUTPUT_STREAM_H_H

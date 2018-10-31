//
// Created by gencom on 4/10/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_INTEGER_OUTPUT_STREAM_H
#define READ_SIGNATURES_SIGNATURE_INTEGER_OUTPUT_STREAM_H


#include "../BitStreams/OutputBitstream.h"
#include "Signature.h"

typedef struct {
    OutputBitstream* outputBitstream;
    int signatureLengthInSymbols;
    int currentSignatureSizeInSymbols;
    uint8_t integerSize;
    uint8_t symbolSize;
    uint8_t currentIntegerContentLength;
    uint64_t totalSize;
} SignatureIntegerOutputStream ;

void initSignatureOutputStream(
        SignatureIntegerOutputStream *signatureIntegerOutputStream,
        OutputBitstream* outputFile,
        int signatureLengthInSymbols,
        uint8_t integerSize,
        uint8_t symbolSize
);

int writeSymbol(
        SignatureIntegerOutputStream* signatureIntegerOutputStream,
        uint8_t symbol
);

int writeTerminator(SignatureIntegerOutputStream* signatureIntegerOutputStream);
int writeBufferSignature(SignatureIntegerOutputStream* signatureIntegerOutputStream);
void writeSignature(SignatureIntegerOutputStream* signatureIntegerOutputStream, Signature* signature);


#endif //READ_SIGNATURES_SIGNATURE_INTEGER_OUTPUT_STREAM_H_H

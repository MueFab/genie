//
// Created by gencom on 29/09/17.
//

#ifndef READ_SIGNATURES_SIGNATURE_INTEGER_H
#define READ_SIGNATURES_SIGNATURE_INTEGER_H

#include <stdint.h>

typedef struct {
    char* data;
    uint32_t dataSizeInBits;
    uint32_t currentBytePosition;
    uint8_t currentBitInByte;
    uint32_t currentBitPosition;
} SignatureInputArbitraryLengthInteger;

typedef struct {
    uint8_t integer;
    uint8_t currentPosition;
} SignatureInputInteger8;

typedef struct {
    uint16_t integer;
    uint8_t currentPosition;
} SignatureInputInteger16;

typedef struct {
    uint32_t integer;
    uint8_t currentPosition;
} SignatureInputInteger32;

void initSignatureInteger8(SignatureInputInteger8* signatureInteger8, uint8_t integer);
void initSignatureInteger16(SignatureInputInteger16* signatureInteger16, uint16_t integer);
void initSignatureInteger32(SignatureInputInteger32* signatureInteger32, uint32_t integer);
int initSignatureArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger,
                                        char* data, uint32_t dataSizeInBits);

int oneSymbolLeft8(SignatureInputInteger8* signatureInteger8, uint8_t lengthSymbol);
int oneSymbolLeft16(SignatureInputInteger16* signatureInteger16, uint8_t lengthSymbol);
int oneSymbolLeft32(SignatureInputInteger32* signatureInteger32, uint8_t lengthSymbol);
int oneSymbolLeftArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger,
                                        uint8_t lengthSymbol);

uint8_t getSymbol8(SignatureInputInteger8* signatureInteger8, uint8_t lengthSymbol);
uint8_t getSymbol16(SignatureInputInteger16* signatureInteger16, uint8_t lengthSymbol);
uint8_t getSymbol32(SignatureInputInteger32* signatureInteger32, uint8_t lengthSymbol);
uint8_t getSymbolArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger,
                                        uint8_t lengthSymbol);

int isTerminator(uint8_t symbol);
void freeSignatureArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger);
#endif  // READ_SIGNATURES_SIGNATURE_INTEGER_H

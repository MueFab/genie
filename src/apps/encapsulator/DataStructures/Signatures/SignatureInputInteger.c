//
// Created by gencom on 29/09/17.
//

#include "SignatureInputInteger.h"
#include <stdlib.h>
#include <string.h>
#include "../../utils.h"

uint8_t createMask(uint8_t lengthSymbol);

int oneSymbolLeft8(SignatureInputInteger8* signatureInteger8, uint8_t lengthSymbol) {
    return (8 - signatureInteger8->currentPosition >= lengthSymbol);
}

int oneSymbolLeft16(SignatureInputInteger16* signatureInteger16, uint8_t lengthSymbol) {
    return (16 - signatureInteger16->currentPosition >= lengthSymbol);
}

int oneSymbolLeft32(SignatureInputInteger32* signatureInteger32, uint8_t lengthSymbol) {
    return (32 - signatureInteger32->currentPosition >= lengthSymbol);
}

int oneSymbolLeftArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger,
                                        uint8_t lengthSymbol) {
    return (signatureInputArbitraryLengthInteger->dataSizeInBits -
            signatureInputArbitraryLengthInteger->currentBitPosition) >= lengthSymbol;
}

void initSignatureInteger8(SignatureInputInteger8* signatureInteger8, uint8_t integer) {
    signatureInteger8->currentPosition = 0;
    signatureInteger8->integer = integer;
}
void initSignatureInteger16(SignatureInputInteger16* signatureInteger16, uint16_t integer) {
    signatureInteger16->currentPosition = 0;
    signatureInteger16->integer = integer;
}
void initSignatureInteger32(SignatureInputInteger32* signatureInteger32, uint32_t integer) {
    signatureInteger32->currentPosition = 0;
    signatureInteger32->integer = integer;
}

int initSignatureArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger,
                                        char* data, uint32_t dataSizeInBits) {
    signatureInputArbitraryLengthInteger->currentBitPosition = 0;
    uint64_t numberOfBytes = bitsToBytes(dataSizeInBits);
    signatureInputArbitraryLengthInteger->data = (char*)malloc(numberOfBytes * sizeof(char));
    if (signatureInputArbitraryLengthInteger->data == NULL) {
        return 0;
    }
    signatureInputArbitraryLengthInteger->dataSizeInBits = dataSizeInBits;
    signatureInputArbitraryLengthInteger->currentBytePosition = 0;
    signatureInputArbitraryLengthInteger->currentBitInByte = 0;
    memcpy(signatureInputArbitraryLengthInteger->data, data, numberOfBytes);

    return 1;
}

uint8_t createMask(uint8_t lengthSymbol) {
    uint8_t mask = 0;
    for (uint8_t symbol_bit_i = 0; symbol_bit_i < lengthSymbol; symbol_bit_i++) {
        mask <<= 1;
        mask |= 0x1;
    }
    return mask;
}

uint8_t getSymbol8(SignatureInputInteger8* signatureInteger8, uint8_t lengthSymbol) {
    uint8_t mask = createMask(lengthSymbol);
    uint8_t bitsToShift = ((uint8_t)8 - signatureInteger8->currentPosition) - lengthSymbol;
    uint8_t shifted_mask = mask << bitsToShift;
    uint8_t shifted_symbol = signatureInteger8->currentPosition & shifted_mask;
    return shifted_symbol >> bitsToShift;
}

uint8_t getSymbol16(SignatureInputInteger16* signatureInteger16, uint8_t lengthSymbol) {
    uint8_t mask = createMask(lengthSymbol);
    uint8_t bitsToShift = ((uint8_t)16 - signatureInteger16->currentPosition) - lengthSymbol;
    uint8_t shifted_mask = mask << bitsToShift;
    uint8_t shifted_symbol = signatureInteger16->currentPosition & shifted_mask;
    return shifted_symbol >> bitsToShift;
}

uint8_t getSymbol32(SignatureInputInteger32* signatureInteger32, uint8_t lengthSymbol) {
    uint8_t mask = createMask(lengthSymbol);
    uint8_t bitsToShift = ((uint8_t)32 - signatureInteger32->currentPosition) - lengthSymbol;
    uint8_t shifted_mask = mask << bitsToShift;
    uint8_t shifted_symbol = signatureInteger32->currentPosition & shifted_mask;
    return shifted_symbol >> bitsToShift;
}

uint8_t getSymbolArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger,
                                        uint8_t lengthSymbol) {
    uint8_t symbol = 0;
    uint8_t remainingToRead = lengthSymbol;
    if ((8 - signatureInputArbitraryLengthInteger->currentBitInByte) < lengthSymbol) {
        uint8_t mask = createMask((uint8_t)8 - signatureInputArbitraryLengthInteger->currentBitInByte);
        symbol |=
            (signatureInputArbitraryLengthInteger->data[signatureInputArbitraryLengthInteger->currentBytePosition] &
             mask);
        remainingToRead -= (uint8_t)8 - signatureInputArbitraryLengthInteger->currentBitInByte;
        symbol <<= remainingToRead;
        signatureInputArbitraryLengthInteger->currentBytePosition++;
        signatureInputArbitraryLengthInteger->currentBitInByte = 0;

        mask = createMask(remainingToRead) << ((uint8_t)8 - remainingToRead);
        symbol |=
            (signatureInputArbitraryLengthInteger->data[signatureInputArbitraryLengthInteger->currentBytePosition] &
             mask) >>
            (8 - remainingToRead);
        signatureInputArbitraryLengthInteger->currentBitInByte += remainingToRead;
        signatureInputArbitraryLengthInteger->currentBitPosition += lengthSymbol;
        return symbol;
    }
    uint8_t mask = createMask(lengthSymbol);
    mask <<= 8 - signatureInputArbitraryLengthInteger->currentBitInByte - lengthSymbol;
    symbol = (uint8_t)(
        signatureInputArbitraryLengthInteger->data[signatureInputArbitraryLengthInteger->currentBytePosition] & mask);
    symbol >>= 8 - signatureInputArbitraryLengthInteger->currentBitInByte - lengthSymbol;

    signatureInputArbitraryLengthInteger->currentBitInByte += lengthSymbol;
    signatureInputArbitraryLengthInteger->currentBitPosition += lengthSymbol;
    if (signatureInputArbitraryLengthInteger->currentBitInByte == 8) {
        signatureInputArbitraryLengthInteger->currentBitInByte = 0;
        signatureInputArbitraryLengthInteger->currentBytePosition++;
    }
    return symbol;
}

int isTerminator(uint8_t symbol) { return symbol == 0; }

void freeSignatureArbitraryLengthInteger(SignatureInputArbitraryLengthInteger* signatureInputArbitraryLengthInteger) {
    free(signatureInputArbitraryLengthInteger->data);
    signatureInputArbitraryLengthInteger->currentBitPosition = 0;
    signatureInputArbitraryLengthInteger->currentBitInByte = 0;
    signatureInputArbitraryLengthInteger->currentBytePosition = 0;
}
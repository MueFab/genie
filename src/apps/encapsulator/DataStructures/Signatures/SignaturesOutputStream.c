//
// Created by gencom on 6/10/17.
//
#include "SignaturesOutputStream.h"
#include <stdlib.h>
#include <utils.h>

int writeConstantLengthSignature(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite);
int writeVariableLengthSignature(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite);
int writeConstantLengthIntegerTooSmall(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite);
int writeVariableLengthIntegerTooSmall(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite);

SignaturesOutputStream* initSignaturesOutputStream(OutputBitstream* outputFile, uint32_t multipleSignatureBase,
                                                   uint8_t signatureSize, uint8_t u_signature_length,
                                                   uint8_t alphabet) {
    SignaturesOutputStream* signatureOutputStream = (SignaturesOutputStream*)malloc(sizeof(SignaturesOutputStream));
    signatureOutputStream->outputBitstream = outputFile;
    signatureOutputStream->multipleSignatureBase = multipleSignatureBase;
    signatureOutputStream->signatureSize = signatureSize;
    signatureOutputStream->u_signature_length = u_signature_length;
    signatureOutputStream->alphabet = alphabet;

    return signatureOutputStream;
}

int writeSignatures(SignaturesOutputStream* signaturesOutputStream, Signatures* signatures) {
    if (signatures->number_signatures != signaturesOutputStream->multipleSignatureBase) {
        if (signatures->number_signatures > 0xffff) {
            fprintf(stderr,
                    "If the number of signatures is different to multiple signature base, then there cannot be "
                    "more then 0xffff signatures");
            return -1;
        }
        uint64_t valueToWrite = 0xffffffffffffffff;
        writeNBitsFromByteArray(signaturesOutputStream->outputBitstream, signaturesOutputStream->signatureSize, 8,
                                (const uint8_t*)&valueToWrite);
        uint16_t castedNumberSignatures = (uint16_t)signatures->number_signatures;
        castedNumberSignatures = nativeToBigEndian16(castedNumberSignatures);
        writeNBitsFromByteArray(signaturesOutputStream->outputBitstream, 16, 2,
                                (const uint8_t*)&castedNumberSignatures);
    }
    for (uint32_t signature_i = 0; signature_i < signatures->number_signatures; signature_i++) {
        Signature* signatureToWrite = signatures->signature[signature_i];
        uint8_t symbolSize = (uint8_t)(signaturesOutputStream->alphabet == 0 ? 3 : 5);

        int resultWrite;
        if (signaturesOutputStream->u_signature_length != 0) {
            if (signaturesOutputStream->u_signature_length != signatureToWrite->signatureLength) {
                fprintf(stderr,
                        "The number of signature of symbols is set as constant, but the signature has a "
                        "different length");
                return -1;
            }
            if (symbolSize > signaturesOutputStream->signatureSize) {
                resultWrite = writeConstantLengthIntegerTooSmall(signaturesOutputStream, signatureToWrite);
            } else {
                resultWrite = writeConstantLengthSignature(signaturesOutputStream, signatureToWrite);
            }
        } else {
            if (symbolSize > signaturesOutputStream->signatureSize) {
                resultWrite = writeVariableLengthIntegerTooSmall(signaturesOutputStream, signatureToWrite);
            } else {
                resultWrite = writeVariableLengthSignature(signaturesOutputStream, signatureToWrite);
            }
        }

        if (resultWrite != 0) {
            return resultWrite;
        }
    }
    return 0;
}

int writeConstantLengthSignature(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite) {
    uint8_t bitsLeftInInteger = signaturesOutputStream->signatureSize;
    uint8_t symbolSize = (uint8_t)(signaturesOutputStream->alphabet == 0 ? 3 : 5);

    uint64_t currentInteger = 0;

    for (uint32_t symbol_i = 0; symbol_i < signatureToWrite->signatureLength; symbol_i++) {
        if (symbolSize > bitsLeftInInteger) {
            currentInteger = nativeToBigEndian64(currentInteger);
            writeNBitsFromByteArray(signaturesOutputStream->outputBitstream, signaturesOutputStream->signatureSize,
                                    sizeof(currentInteger), (const uint8_t*)&currentInteger);
            bitsLeftInInteger = signaturesOutputStream->signatureSize;
            currentInteger = 0;
        }
        uint8_t symbolRepresentation = signatureToWrite->symbols[symbol_i];
        currentInteger = currentInteger << symbolSize | symbolRepresentation;
        bitsLeftInInteger -= symbolSize;
    }
    currentInteger = nativeToBigEndian64(currentInteger);
    writeNBitsFromByteArray(signaturesOutputStream->outputBitstream, signaturesOutputStream->signatureSize,
                            sizeof(currentInteger), (const uint8_t*)&currentInteger);
    return 0;
}

int writeConstantLengthIntegerTooSmall(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite) {
    uint8_t symbolSize = (uint8_t)(signaturesOutputStream->alphabet == 0 ? 3 : 5);
    for (uint32_t symbol_i = 0; symbol_i < signatureToWrite->signatureLength; symbol_i++) {
        uint8_t symbol = signatureToWrite->symbols[symbol_i];

        uint8_t bitsLeftToWrite = symbolSize;
        while (bitsLeftToWrite > 0) {
            if (bitsLeftToWrite >= signaturesOutputStream->signatureSize) {
                for (uint8_t i = 0; i < signaturesOutputStream->signatureSize; i++) {
                    writeBit(signaturesOutputStream->outputBitstream, symbol >> (bitsLeftToWrite - 1));
                    uint8_t mask = ~(uint8_t)(1 << (bitsLeftToWrite - 1));
                    symbol &= mask;
                    bitsLeftToWrite--;
                }
            } else {
                for (uint8_t i = 0; i < (signaturesOutputStream->signatureSize - bitsLeftToWrite); i++) {
                    writeBit(signaturesOutputStream->outputBitstream, 0);
                }
                for (uint8_t i = 0; i < bitsLeftToWrite; i++) {
                    writeBit(signaturesOutputStream->outputBitstream, symbol >> (bitsLeftToWrite - i - 1));
                    uint8_t mask = ~(uint8_t)(1 << (bitsLeftToWrite - 1));
                    symbol &= mask;
                }
                bitsLeftToWrite = 0;
            }
        }
    }
    return 0;
}

int writeVariableLengthIntegerTooSmall(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite) {
    uint8_t symbolSize = (uint8_t)(signaturesOutputStream->alphabet == 0 ? 3 : 5);
    for (uint8_t symbol_i = 0; symbol_i < (uint64_t)signatureToWrite->signatureLength + 1; symbol_i++) {
        uint8_t symbol;
        if (symbol_i == signatureToWrite->signatureLength) {
            symbol = 0;
        } else {
            symbol = (uint8_t)(signatureToWrite->symbols[symbol_i] + 1);
        }

        uint8_t bitsLeftToWrite = symbolSize;
        while (bitsLeftToWrite > 0) {
            if (bitsLeftToWrite >= signaturesOutputStream->signatureSize) {
                for (uint8_t i = 0; i < signaturesOutputStream->signatureSize; i++) {
                    writeBit(signaturesOutputStream->outputBitstream, symbol >> (bitsLeftToWrite - 1));
                    uint8_t mask = ~(uint8_t)(1 << (bitsLeftToWrite - 1));
                    symbol &= mask;
                    bitsLeftToWrite--;
                }
            } else {
                for (uint8_t i = 0; i < (signaturesOutputStream->signatureSize - bitsLeftToWrite); i++) {
                    writeBit(signaturesOutputStream->outputBitstream, 1);
                }
                for (uint8_t i = 0; i < bitsLeftToWrite; i++) {
                    writeBit(signaturesOutputStream->outputBitstream, symbol >> (bitsLeftToWrite - i - 1));
                    uint8_t mask = ~(uint8_t)(1 << (bitsLeftToWrite - 1));
                    symbol &= mask;
                }
                bitsLeftToWrite = 0;
            }
        }
    }
    return 0;
}

int writeVariableLengthSignature(SignaturesOutputStream* signaturesOutputStream, Signature* signatureToWrite) {
    uint8_t symbolSize = (uint8_t)(signaturesOutputStream->alphabet == 0 ? 3 : 5);
    uint8_t bitsLeftInInteger =
        signaturesOutputStream->signatureSize - signaturesOutputStream->signatureSize % symbolSize;
    uint8_t maxSymbolsPerInteger = signaturesOutputStream->signatureSize / symbolSize;
    uint64_t currentInteger = 0xffffffffffffffff;

    for (uint8_t symbol_i = 0; symbol_i < ((uint64_t)signatureToWrite->signatureLength + 1); symbol_i++) {
        uint8_t valueToWrite;
        if (symbol_i == signatureToWrite->signatureLength) {
            valueToWrite = 0;
        } else {
            valueToWrite = (uint8_t)(signatureToWrite->symbols[symbol_i] + 1);
        }
        if (bitsLeftInInteger == 0) {
            currentInteger = nativeToBigEndian64(currentInteger);
            writeNBitsFromByteArray(signaturesOutputStream->outputBitstream, signaturesOutputStream->signatureSize,
                                    sizeof(currentInteger), (const uint8_t*)&currentInteger);
            bitsLeftInInteger =
                signaturesOutputStream->signatureSize - signaturesOutputStream->signatureSize % symbolSize;
            currentInteger = 0xffffffffffffffff;
        }

        currentInteger = currentInteger << symbolSize | valueToWrite;
        bitsLeftInInteger -= symbolSize;
    }
    currentInteger = nativeToBigEndian64(currentInteger);
    writeNBitsFromByteArray(signaturesOutputStream->outputBitstream, signaturesOutputStream->signatureSize,
                            sizeof(currentInteger), (const uint8_t*)&currentInteger);
    return 0;
}

void freeSignaturesOutputStream(SignaturesOutputStream* signaturesOutputStream) { free(signaturesOutputStream); }

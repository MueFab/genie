//
// Created by gencom on 29/09/17.
//

#include "SignaturesInputStream.h"
#include <stdlib.h>
#include <string.h>
#include "../../utils.h"
#include "Signatures.h"

Signature* readFixedSizeSignature(SignaturesInputStream* signaturesInputStream);
Signature* readVariableSizeSignature(SignaturesInputStream* signaturesInputStream);
uint8_t getSymbolFixedLength(SignaturesInputStream* stream, uint8_t symbolsLeft);
uint8_t getSymbolVariableLength(SignaturesInputStream* stream);
uint8_t getSymbolTooSmallInteger(SignaturesInputStream* stream);
Signature* readFixedSizeTooSmallIntegerSignature(SignaturesInputStream* signaturesInputStream);
Signature* readVariableSizeTooSmallIntegerSignature(SignaturesInputStream* signaturesInputStream);

SignaturesInputStream* initSignatureInputStream(InputBitstream* inputBitstream, uint8_t signatureSize,
                                                uint8_t u_signature_length, uint32_t multipleSignatureBase,
                                                uint8_t alphabet) {
    SignaturesInputStream* signaturesInputStream = (SignaturesInputStream*)malloc(sizeof(SignaturesInputStream));
    if (signaturesInputStream == NULL) {
        return NULL;
    }

    signaturesInputStream->inputBitstream = inputBitstream;
    signaturesInputStream->signatureSize = signatureSize;
    signaturesInputStream->u_signature_length = u_signature_length;
    signaturesInputStream->numberSignatures = multipleSignatureBase;
    signaturesInputStream->alphabet = alphabet;
    signaturesInputStream->posCurrentInteger = 64;
    signaturesInputStream->currentInteger = 0;

    readBitsToByteArray(signaturesInputStream->inputBitstream, signatureSize, 8,
                        (uint8_t*)&(signaturesInputStream->currentInteger));
    signaturesInputStream->currentInteger = bigToNativeEndian64(signaturesInputStream->currentInteger);

    uint8_t symbolSize = (uint8_t)(signaturesInputStream->alphabet == 0 ? 3 : 5);

    if (signaturesInputStream->currentInteger == (uint64_t)(1L << signatureSize) - 1L) {
        uint16_t numberSignatures;
        readNBitsBigToNativeEndian16(inputBitstream, 16, &numberSignatures);
        signaturesInputStream->numberSignatures = numberSignatures;
    } else {
        if (signatureSize >= symbolSize) {
            uint8_t possibleSymbolsPerInteger = signatureSize / symbolSize;
            uint8_t symbolsInFirstInteger = possibleSymbolsPerInteger;
            if (u_signature_length != 0) {
                if (u_signature_length < symbolsInFirstInteger) {
                    symbolsInFirstInteger = u_signature_length;
                }
            }
            signaturesInputStream->posCurrentInteger =
                (uint8_t)(64 - signatureSize + signatureSize % symbolSize +
                          (possibleSymbolsPerInteger - symbolsInFirstInteger) * symbolSize);
        } else {
            signaturesInputStream->posCurrentInteger = (uint8_t)(64 - signatureSize);
        }
    }

    return signaturesInputStream;
}

Signatures* readSignatures(SignaturesInputStream* signaturesInputStream) {
    Signatures* signatures = initSignatures(signaturesInputStream->numberSignatures);
    uint8_t symbolSize = (uint8_t)(signaturesInputStream->alphabet == 0 ? 3 : 5);
    for (uint32_t signature_i = 0; signature_i < signaturesInputStream->numberSignatures; signature_i++) {
        Signature* signature;
        if (signaturesInputStream->u_signature_length != 0) {
            if (symbolSize <= signaturesInputStream->signatureSize) {
                signature = readFixedSizeSignature(signaturesInputStream);
            } else {
                signature = readFixedSizeTooSmallIntegerSignature(signaturesInputStream);
            }
        } else {
            if (symbolSize <= signaturesInputStream->signatureSize) {
                signature = readVariableSizeSignature(signaturesInputStream);
            } else {
                signature = readVariableSizeTooSmallIntegerSignature(signaturesInputStream);
            }
        }
        setSignature(signatures, signature);
    }

    return signatures;
}

Signature* readFixedSizeSignature(SignaturesInputStream* signaturesInputStream) {
    uint8_t* data = (uint8_t*)malloc(signaturesInputStream->u_signature_length * sizeof(uint8_t));
    for (uint8_t symbol_i = 0; symbol_i < signaturesInputStream->u_signature_length; symbol_i++) {
        data[symbol_i] = getSymbolFixedLength(signaturesInputStream,
                                              (uint8_t)(signaturesInputStream->u_signature_length - symbol_i));
    }
    Signature* signature;
    initSignatureWithData(&signature, data, signaturesInputStream->u_signature_length);
    free(data);
    return signature;
}

uint8_t getSymbolFixedLength(SignaturesInputStream* stream, uint8_t symbolsLeft) {
    uint8_t symbolSize = (uint8_t)(stream->alphabet == 0 ? 3 : 5);

    uint64_t mask = (uint64_t)(symbolSize == 3 ? 0x07 : 0x1f);

    if (stream->posCurrentInteger == 64) {
        stream->currentInteger = 0;
        readBitsToByteArray(stream->inputBitstream, stream->signatureSize, 8, (uint8_t*)&(stream->currentInteger));
        stream->currentInteger = bigToNativeEndian64(stream->currentInteger);
        if (symbolsLeft >= stream->signatureSize / symbolSize) {
            stream->posCurrentInteger = (uint8_t)(64 - stream->signatureSize + stream->signatureSize % symbolSize);
        } else {
            stream->posCurrentInteger = (uint8_t)(64 - symbolSize * symbolsLeft);
        }
    }

    mask = mask << (64 - stream->posCurrentInteger - symbolSize);
    uint64_t symbol = stream->currentInteger & mask;
    symbol = symbol >> (64 - stream->posCurrentInteger - symbolSize);
    stream->posCurrentInteger += symbolSize;

    return (uint8_t)symbol;
}

uint8_t getSymbolVariableLength(SignaturesInputStream* stream) {
    uint8_t symbolSize = (uint8_t)(stream->alphabet == 0 ? 3 : 5);
    uint64_t mask = (uint8_t)(symbolSize == 3 ? 0x07 : 0x1f);
    uint64_t paddingSignalizer = mask;
    uint64_t returnValue = mask;

    while (returnValue == paddingSignalizer) {
        if (stream->posCurrentInteger == 64) {
            stream->currentInteger = 0;
            readBitsToByteArray(stream->inputBitstream, stream->signatureSize, 8, (uint8_t*)&(stream->currentInteger));
            stream->currentInteger = bigToNativeEndian64(stream->currentInteger);
            stream->posCurrentInteger = (uint8_t)(64 - stream->signatureSize + stream->signatureSize % symbolSize);
        }

        mask = paddingSignalizer << (64 - stream->posCurrentInteger - symbolSize);
        uint64_t symbol = stream->currentInteger & mask;
        symbol = symbol >> (64 - stream->posCurrentInteger - symbolSize);
        stream->posCurrentInteger += symbolSize;

        returnValue = (uint8_t)symbol;
    }

    return returnValue;
}

Signature* readVariableSizeSignature(SignaturesInputStream* signaturesInputStream) {
    uint32_t allocatedLength = 65536;
    uint32_t currentLength = 0;
    uint8_t* data = (uint8_t*)malloc(allocatedLength * sizeof(uint8_t));
    uint8_t symbol = getSymbolVariableLength(signaturesInputStream);

    while (symbol != 0) {
        if (currentLength == allocatedLength) {
            data = realloc(data, allocatedLength * 2 * sizeof(uint8_t));
            allocatedLength *= 2;
        }
        data[currentLength] = (uint8_t)(symbol - 1);
        currentLength++;
        symbol = getSymbolVariableLength(signaturesInputStream);
    }

    Signature* signature;
    initSignatureWithData(&signature, data, currentLength);

    free(data);
    return signature;
}

void freeSignatureInputStream(SignaturesInputStream* signaturesInputStream) { free(signaturesInputStream); }

uint8_t getSymbolTooSmallInteger(SignaturesInputStream* stream) {
    uint8_t symbolSize = (uint8_t)(stream->alphabet == 0 ? 3 : 5);
    uint8_t bitsLeftToRead = symbolSize;
    uint64_t symbol = 0;

    uint8_t mask = (uint8_t)(symbolSize == 3 ? 0x07 : 0x1f);

    while (bitsLeftToRead > 0) {
        if (stream->posCurrentInteger == 64) {
            stream->currentInteger = 0;
            readBitsToByteArray(stream->inputBitstream, stream->signatureSize, 8, (uint8_t*)&(stream->currentInteger));
            stream->currentInteger = bigToNativeEndian64(stream->currentInteger);
            if (bitsLeftToRead >= stream->signatureSize) {
                stream->posCurrentInteger = (uint8_t)(64 - stream->signatureSize);
            } else {
                stream->posCurrentInteger = (uint8_t)(64 - bitsLeftToRead);
            }
        }

        uint8_t maskSize;
        if (bitsLeftToRead > stream->signatureSize) {
            maskSize = stream->signatureSize;
        } else {
            maskSize = bitsLeftToRead;
        }

        mask = (uint8_t) ~((uint64_t)(~0L) << maskSize);
        symbol = symbol << maskSize;
        symbol = symbol | (stream->currentInteger & mask);
        bitsLeftToRead -= maskSize;

        stream->posCurrentInteger += maskSize;
    }

    return (uint8_t)symbol;
}

Signature* readFixedSizeTooSmallIntegerSignature(SignaturesInputStream* signaturesInputStream) {
    uint8_t* data = (uint8_t*)malloc(signaturesInputStream->u_signature_length * sizeof(uint8_t));
    for (uint8_t symbol_i = 0; symbol_i < signaturesInputStream->u_signature_length; symbol_i++) {
        data[symbol_i] = getSymbolTooSmallInteger(signaturesInputStream);
    }
    Signature* signature;
    initSignatureWithData(&signature, data, signaturesInputStream->u_signature_length);

    return signature;
}

Signature* readVariableSizeTooSmallIntegerSignature(SignaturesInputStream* signaturesInputStream) {
    uint32_t allocatedLength = 65536;
    uint32_t currentLength = 0;
    uint8_t* data = (uint8_t*)malloc(allocatedLength * sizeof(uint8_t));
    uint8_t symbol = getSymbolTooSmallInteger(signaturesInputStream);

    while (symbol != 0) {
        if (currentLength == allocatedLength) {
            data = realloc(data, allocatedLength * 2 * sizeof(uint8_t));
            allocatedLength *= 2;
        }
        data[currentLength] = (uint8_t)(symbol - 1);
        currentLength++;
        symbol = getSymbolTooSmallInteger(signaturesInputStream);
    }

    Signature* signature;
    initSignatureWithData(&signature, data, currentLength);

    free(data);
    return signature;
}

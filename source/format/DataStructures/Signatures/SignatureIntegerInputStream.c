//
// Created by gencom on 29/09/17.
//

#include <stdlib.h>
#include <string.h>
#include "SignatureIntegerInputStream.h"
#include "../../utils.h"


int initSignatureInputStream(SignatureArbitraryLengthIntegerInputStream* signatureArbitraryLengthIntegerInputStream,
                             InputBitstream* inputFile, uint8_t integerLength, uint8_t symbolLength,
                             uint8_t signatureSizeInSymbols){
    signatureArbitraryLengthIntegerInputStream->inputFile=inputFile;

    signatureArbitraryLengthIntegerInputStream->symbolLength=symbolLength;
    signatureArbitraryLengthIntegerInputStream->integerLength=integerLength;
    signatureArbitraryLengthIntegerInputStream->currentSizeInSymbols=0;

    uint8_t integerLengthInBytes = (uint8_t) bitsToBytes(integerLength);
    signatureArbitraryLengthIntegerInputStream->buffer = (char*)malloc(integerLengthInBytes*sizeof(uint8_t));
    if (signatureArbitraryLengthIntegerInputStream->buffer == NULL){
        return 0;
    }
    if (!readNBitsShift(
            signatureArbitraryLengthIntegerInputStream->inputFile,
            integerLength,
            signatureArbitraryLengthIntegerInputStream->buffer
    )){
        return 0;
    };
    signatureArbitraryLengthIntegerInputStream->signatureSizeInSymbols = signatureSizeInSymbols;
    initSignatureArbitraryLengthInteger(
            &(signatureArbitraryLengthIntegerInputStream->signatureInteger),
            signatureArbitraryLengthIntegerInputStream->buffer,
            integerLength
    );
    return 1;

}

int initSignatureInputStreamAndFirstInteger(SignatureArbitraryLengthIntegerInputStream* signatureArbitraryLengthIntegerInputStream,
                                            InputBitstream* inputFile, uint8_t integerLength, uint8_t symbolLength,
                                            uint8_t signatureSizeInSymbols, uint8_t* integerData){
    signatureArbitraryLengthIntegerInputStream->inputFile=inputFile;

    signatureArbitraryLengthIntegerInputStream->symbolLength=symbolLength;
    signatureArbitraryLengthIntegerInputStream->integerLength=integerLength;
    signatureArbitraryLengthIntegerInputStream->currentSizeInSymbols=0;

    uint8_t integerLengthInBytes = (uint8_t) bitsToBytes(integerLength);
    signatureArbitraryLengthIntegerInputStream->buffer = (char*)malloc(integerLengthInBytes*sizeof(uint8_t));
    if (signatureArbitraryLengthIntegerInputStream->buffer == NULL){
        return 0;
    }
    memcpy(signatureArbitraryLengthIntegerInputStream->buffer, integerData, integerLengthInBytes);
    signatureArbitraryLengthIntegerInputStream->signatureSizeInSymbols = signatureSizeInSymbols;
    initSignatureArbitraryLengthInteger(
            &(signatureArbitraryLengthIntegerInputStream->signatureInteger),
            signatureArbitraryLengthIntegerInputStream->buffer,
            integerLength
    );
    return 1;
}

int endOfStream(SignatureArbitraryLengthIntegerInputStream* signatureIntegerStream){
    return eofInputBitstream(signatureIntegerStream->inputFile);

}



uint8_t getNextSymbol(SignatureArbitraryLengthIntegerInputStream* signatureIntegerStream){
    if (!oneSymbolLeftArbitraryLengthInteger(
            &(signatureIntegerStream->signatureInteger),
            signatureIntegerStream->symbolLength
    )){
        readNBitsShift(
                signatureIntegerStream->inputFile,
                signatureIntegerStream->integerLength,
                signatureIntegerStream->buffer
        );

        freeSignatureArbitraryLengthInteger(&(signatureIntegerStream->signatureInteger));

        initSignatureArbitraryLengthInteger(
                &(signatureIntegerStream->signatureInteger),
                signatureIntegerStream->buffer,
                signatureIntegerStream->integerLength
        );
    };

    uint8_t symbol = getSymbolArbitraryLengthInteger(
            &(signatureIntegerStream->signatureInteger),
            signatureIntegerStream->symbolLength
    );
    signatureIntegerStream->currentSizeInSymbols++;



    return symbol;
}

Signature getSignatureFromInputStream(SignatureArbitraryLengthIntegerInputStream *signatureIntegerStream){
    Signature signature;
    initSignature(&signature);
    if (signatureIntegerStream->signatureSizeInSymbols != 0){
        for(int symbol_i = 0; symbol_i<signatureIntegerStream->signatureSizeInSymbols; symbol_i++){
            uint8_t symbol = getNextSymbol(signatureIntegerStream);
            addSymbol(&signature, symbol);
        }
    }else{
        uint8_t symbol = getNextSymbol(signatureIntegerStream);
        while(!isTerminator(symbol)){
            addSymbol(&signature,symbol);
            symbol =  getNextSymbol(signatureIntegerStream);
        }
    }
    return signature;
}

int getNewInteger(SignatureArbitraryLengthIntegerInputStream *stream) {
    freeSignatureArbitraryLengthInteger(&(stream->signatureInteger));


    if (!readNBitsShift(
            stream->inputFile,
            stream->integerLength,
            stream->buffer
    )){
        return 0;
    };

    initSignatureArbitraryLengthInteger(
            &(stream->signatureInteger),
            stream->buffer,
            stream->integerLength
    );
    return 0;
}

void freeSignatureInputStream(SignatureArbitraryLengthIntegerInputStream* signatureArbitraryLengthIntegerInputStream){
    freeSignatureArbitraryLengthInteger(&(signatureArbitraryLengthIntegerInputStream->signatureInteger));
    free(signatureArbitraryLengthIntegerInputStream->buffer);
}
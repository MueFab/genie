//
// Created by gencom on 6/10/17.
//
#include "SignatureIntegerOutputStream.h"

void initSignatureOutputStream(
        SignatureIntegerOutputStream *signatureIntegerOutputStream,
        OutputBitstream* outputBitstream,
        int signatureLengthInSymbols,
        uint8_t integerSize,
        uint8_t symbolSize
){
    signatureIntegerOutputStream->outputBitstream = outputBitstream;
    signatureIntegerOutputStream->integerSize = integerSize;
    signatureIntegerOutputStream->symbolSize = symbolSize;
    signatureIntegerOutputStream->signatureLengthInSymbols = signatureLengthInSymbols;
    signatureIntegerOutputStream->currentIntegerContentLength = 0;
    signatureIntegerOutputStream->currentSignatureSizeInSymbols = 0;
    signatureIntegerOutputStream->totalSize = 0;
}

int writeSymbol(
        SignatureIntegerOutputStream* signatureIntegerOutputStream,
        uint8_t symbol
){
    if (!writeNBitsShift(
            signatureIntegerOutputStream->outputBitstream,
            signatureIntegerOutputStream->symbolSize,
            (char *) &symbol
    )){
        return 0;
    };
    signatureIntegerOutputStream->totalSize += signatureIntegerOutputStream->symbolSize;

    signatureIntegerOutputStream->currentIntegerContentLength += signatureIntegerOutputStream->symbolSize;
    signatureIntegerOutputStream->currentSignatureSizeInSymbols++;

    uint8_t sizeRemaining = signatureIntegerOutputStream->integerSize -
            signatureIntegerOutputStream->currentIntegerContentLength;

    if(signatureIntegerOutputStream->signatureLengthInSymbols ==
            signatureIntegerOutputStream->currentSignatureSizeInSymbols
       || sizeRemaining < signatureIntegerOutputStream->symbolSize){

        uint8_t toWrite = sizeRemaining;

        while(toWrite>0) {
            uint8_t nextWriteSize;
            if(toWrite>16){
                nextWriteSize = 16;
            }else{
                nextWriteSize = toWrite;
            }
            uint16_t zeroBuffer = 0;
            if (!writeNBitsShift(
                    signatureIntegerOutputStream->outputBitstream,
                    toWrite,
                    (char *) &zeroBuffer
            )) {
                return 0;
            };

            signatureIntegerOutputStream->totalSize += toWrite;
            toWrite -= nextWriteSize;
        }


        signatureIntegerOutputStream->currentIntegerContentLength = 0;

        if(signatureIntegerOutputStream->signatureLengthInSymbols ==
           signatureIntegerOutputStream->currentSignatureSizeInSymbols){
            signatureIntegerOutputStream->currentSignatureSizeInSymbols=0;
        }

    }
}

int writeTerminator(SignatureIntegerOutputStream* signatureIntegerOutputStream){
    if(signatureIntegerOutputStream->signatureLengthInSymbols != 0) return 1;
    uint64_t zeroBuffer = 0;
    if (!writeNBitsShift(
            signatureIntegerOutputStream->outputBitstream,
            signatureIntegerOutputStream->symbolSize,
            (char*)&zeroBuffer
    )){
        return 0;
    };
    signatureIntegerOutputStream->totalSize += signatureIntegerOutputStream->symbolSize;
    signatureIntegerOutputStream->currentIntegerContentLength += signatureIntegerOutputStream->symbolSize;
    uint8_t sizeRemaining = signatureIntegerOutputStream->integerSize -
            signatureIntegerOutputStream->currentIntegerContentLength;
    while(sizeRemaining>0){
        uint8_t toWrite;
        if(sizeRemaining>64){
            toWrite = 64;
        }else{
            toWrite = sizeRemaining;
        }

        if (!writeNBitsShift(
                signatureIntegerOutputStream->outputBitstream,
                toWrite,
                (char*)&zeroBuffer
        )){
            return 0;
        };

        sizeRemaining -= toWrite;
        signatureIntegerOutputStream->currentIntegerContentLength += toWrite;
        signatureIntegerOutputStream->totalSize += toWrite;
    }
    signatureIntegerOutputStream->currentIntegerContentLength = 0;
    return 1;
}

int writeBufferSignature(SignatureIntegerOutputStream* signatureIntegerOutputStream){
    //fprintf(stderr,"first signature: %li\n", signatureIntegerOutputStream->totalSize);
    return writeBuffer(signatureIntegerOutputStream->outputBitstream);
}

void writeSignature(SignatureIntegerOutputStream* signatureIntegerOutputStream, Signature* signature){
    for(int i=0;i<signature->signatureLength;i++){
        writeSymbol(signatureIntegerOutputStream, signature->symbols[i]);
    }
    writeTerminator(signatureIntegerOutputStream);
}
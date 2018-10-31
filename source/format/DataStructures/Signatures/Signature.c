//
// Created by gencom on 29/09/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Signature.h"

int initSignature(Signature* signature){
    signature->symbols = (uint8_t*) malloc(20*sizeof(uint8_t));
    if (signature->symbols == NULL){
        signature->allocatedLength = 0;
        signature->signatureLength = 0;
        return -1;
    }
    signature->allocatedLength = 20;
    signature->signatureLength = 0;
    return 0;
}

int initSignatureWithData(Signature* signature, uint8_t* data, uint16_t signatureLength){
    signature->symbols = (uint8_t*) malloc(signatureLength*sizeof(uint8_t));
    if (signature->symbols == NULL){
        signature->allocatedLength = 0;
        signature->signatureLength = 0;
        return -1;
    }
    signature->allocatedLength = signatureLength;
    signature->signatureLength = signatureLength;
    memcpy(signature->symbols, data, signatureLength);
    return 0;
}

int addSymbol(Signature* signature, uint8_t symbol){
    if(signature->signatureLength == signature->allocatedLength){
        void* newBuffer = realloc(signature->symbols, signature->allocatedLength+20);
        if(newBuffer == NULL){
            return 0;
        }
        signature->symbols = (uint8_t*) newBuffer;
        signature->allocatedLength+=20;
    }
    signature->symbols[signature->signatureLength] = symbol;
    signature->signatureLength++;
}

void printSignature(Signature* signature){
    for(int i=0;i<signature->signatureLength;i++){
        printf("%u\n",signature->symbols[i]);
    }
}

void freeSignature(Signature* signature){
    free(signature->symbols);
    signature->signatureLength=0;
    signature->allocatedLength=0;
}
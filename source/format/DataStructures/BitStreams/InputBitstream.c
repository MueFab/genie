//
// Created by gencom on 31/07/17.
//

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "InputBitstream.h"
#include "../../utils.h"

bool readNBits(InputBitstream *inputBitstream, uint32_t n, char *value);

bool forwardUntilAligned(InputBitstream* inputBitstream){
    uint8_t buffer;
    while(inputBitstream->currentPositionInChar!=0 && inputBitstream->currentPositionInChar!=8){
        if(!readBit(inputBitstream,&buffer)){
            return false;
        };
    }
    return true;
}

bool readBit(InputBitstream* inputBitstream, uint8_t* value){
    if(inputBitstream->currentPositionInChar==8){
        uint8_t fileReadValue;
        bool success = fread(&fileReadValue,sizeof(fileReadValue),1,inputBitstream->input)==1;
        if (!success){
            return false;
        }
        inputBitstream->currentPositionInChar=0;
        inputBitstream->currentChar=fileReadValue;
    }


    uint8_t mask=1;
    mask <<= (7-inputBitstream->currentPositionInChar);
    *value = inputBitstream->currentChar & mask;

    inputBitstream->currentPositionInChar++;
    return true;
}

bool readNBitsShift(InputBitstream *inputBitstream, uint32_t n, char *value){
    if (!readNBits(inputBitstream,n,value)){
        return false;
    }
    int numberBytes = 1;
    uint8_t numberBits = (uint8_t) 8-(uint8_t) n;
    shiftRight((uint8_t* )value,numberBytes,numberBits);
    return true;
}

bool readBytes(InputBitstream *inputBitstream, uint32_t n, char *value){
    return readNBits(inputBitstream, n*8, value);
}

bool readNBits(InputBitstream *inputBitstream, uint32_t n, char *value){
    uint8_t bufferValue=0;

    int current_bit = 0;
    int current_byte = 0;

    for(int i=0; i<n; i++){
        if(!readBit(inputBitstream, &bufferValue)){
            return false;
        }

        if(bufferValue != 0){
            value[current_byte] |= ((uint8_t) 1)<<(7-current_bit);
        }else{
            value[current_byte] &= ~(((uint8_t) 1)<<(7-current_bit));
        }
        current_bit++;
        if(current_bit==8){
            current_byte++;
            current_bit=0;
        }
    }
    return true;
}

bool readNBitsShiftAndConvertLittleToNativeEndian16(InputBitstream *inputBitstream, uint8_t n, char *value){
    uint16_t buffer;
    bool result = readNBits(inputBitstream, n, (char*) &buffer);
    if (!result){
        return result;
    }
    buffer >>= (16-n);
    buffer = littleToNativeEndian16(buffer);
    memcpy(value,&buffer,2);
    return true;
}
bool readNBitsShiftAndConvertLittleToNativeEndian32(InputBitstream *inputBitstream, uint8_t n, char *value){
    uint32_t buffer=0;
    bool result = readNBits(inputBitstream, n, (char*) &buffer);
    if (!result){
        return result;
    }
    buffer >>= (32-n);
    buffer = littleToNativeEndian32(buffer);
    memcpy(value,&buffer,4);
    return true;
}
bool readNBitsShiftAndConvertLittleToNativeEndian64(InputBitstream *inputBitstream, uint8_t n, char *value){
    uint64_t buffer;
    bool result = readNBits(inputBitstream, n, (char*) &buffer);
    if (!result){
        return result;
    }
    buffer >>= (64-n);
    buffer = littleToNativeEndian64(buffer);
    memcpy(value,&buffer,8);
    return true;
}

bool readNBitsShiftAndConvertBigToNativeEndian16(InputBitstream *inputBitstream, uint8_t n, char *value){
    uint16_t buffer;
    bool result = readNBits(inputBitstream, n, (char*) &buffer);

    if (!result){
        return result;
    }
    shiftRight((uint8_t *) &buffer, 2, (uint8_t) 16-n);
    buffer = bigToNativeEndian16(buffer);
    memcpy(value,&buffer,2);
    return true;
}
bool readNBitsShiftAndConvertBigToNativeEndian32(InputBitstream *inputBitstream, uint8_t n, char *value){
    uint32_t buffer=0;
    bool result = readNBits(inputBitstream, n, (char*) &buffer);
    if (!result){
        return result;
    }
    shiftRight((uint8_t *) &buffer, 4, (uint8_t) 32-n);
    buffer = bigToNativeEndian32(buffer);
    memcpy(value,&buffer,4);
    return true;
}
bool readNBitsShiftAndConvertBigToNativeEndian64(InputBitstream *inputBitstream, uint8_t n, char *value){
    uint64_t buffer;
    bool result = readNBits(inputBitstream, n, (char*) &buffer);
    if (!result){
        return result;
    }
    buffer >>= (64-n);
    buffer = bigToNativeEndian64(buffer);
    memcpy(value,&buffer,8);
    return true;
}

bool initializeInputBitstreamFromFilename(InputBitstream *inputBitstream, const char *filename){
    inputBitstream->currentPositionInChar=8;
    inputBitstream->input = fopen(filename,"rb");
    if (inputBitstream->input ==NULL) {
        printf("Error %s \n", strerror(errno));
    }
    return inputBitstream->input != NULL;
}

bool initializeInputBitstream(InputBitstream *inputBitstream, FILE* inputFile){
    inputBitstream->currentPositionInChar=8;
    inputBitstream->input = inputFile;
    return inputBitstream->input != NULL;
}

bool eofInputBitstream(InputBitstream *inputBitstream){
    return (bool)feof(inputBitstream->input);
}
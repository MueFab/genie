//
// Created by gencom on 31/07/17.
//

#ifndef BITSTREAMIO_INPUTBITSTREAM_H
#define BITSTREAMIO_INPUTBITSTREAM_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct{
    unsigned char currentChar;
    uint8_t currentPositionInChar;
    FILE* input;
} InputBitstream;

bool forwardUntilAligned(InputBitstream* inputBitstream);
bool readBit(InputBitstream* inputBitstream, uint8_t* value);
bool readNBitsShift(InputBitstream *inputBitstream, uint32_t n, char *value);
bool readBytes(InputBitstream *inputBitstream, uint32_t n, char *value);
bool readNBitsShiftAndConvertLittleToNativeEndian16(InputBitstream *inputBitstream, uint8_t n, char *value);
bool readNBitsShiftAndConvertLittleToNativeEndian32(InputBitstream *inputBitstream, uint8_t n, char *value);
bool readNBitsShiftAndConvertLittleToNativeEndian64(InputBitstream *inputBitstream, uint8_t n, char *value);
bool readNBitsShiftAndConvertBigToNativeEndian16(InputBitstream *inputBitstream, uint8_t n, char *value);
bool readNBitsShiftAndConvertBigToNativeEndian32(InputBitstream *inputBitstream, uint8_t n, char *value);
bool readNBitsShiftAndConvertBigToNativeEndian64(InputBitstream *inputBitstream, uint8_t n, char *value);
bool initializeInputBitstreamFromFilename(InputBitstream *inputBitstream, const char *filename);
bool initializeInputBitstream(InputBitstream *inputBitstream, FILE* inputFile);
bool eofInputBitstream(InputBitstream *inputBitstream);

#endif //BITSTREAMIO_INPUTBITSTREAM_H

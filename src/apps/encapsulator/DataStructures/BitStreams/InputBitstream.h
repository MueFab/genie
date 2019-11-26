//
// Created by gencom on 31/07/17.
//

#ifndef BITSTREAMIO_INPUTBITSTREAM_H
#define BITSTREAMIO_INPUTBITSTREAM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    unsigned char currentChar;
    uint8_t currentPositionInChar;
    FILE *input;
} InputBitstream;

bool forwardUntilAligned(InputBitstream *inputBitstream);
bool readBit(InputBitstream *inputBitstream, uint8_t *value);
bool readNBitsShift(InputBitstream *inputBitstream, uint32_t n, char *value);
bool readNBits8(InputBitstream *inputBitstream, uint8_t n, uint8_t *value);
bool readBytes(InputBitstream *inputBitstream, uint32_t n, char *value);
bool readBitsToByteArray(InputBitstream *inputBitStream, uint64_t numBits, uint64_t sizeByteArrayInBytes,
                         uint8_t *byteArray);
bool readNBitsBigToNativeEndian16(InputBitstream *inputBitstream, uint8_t n, uint16_t *value);
bool readNBitsBigToNativeEndian32(InputBitstream *inputBitstream, uint8_t n, uint32_t *value);
bool readNBitsBigToNativeEndian64(InputBitstream *inputBitstream, uint8_t n, uint64_t *value);
bool initializeInputBitstreamFromFilename(InputBitstream *inputBitstream, const char *filename);
bool initializeInputBitstream(InputBitstream *inputBitstream, FILE *inputFile);
bool eofInputBitstream(InputBitstream *inputBitstream);

#endif  // BITSTREAMIO_INPUTBITSTREAM_H

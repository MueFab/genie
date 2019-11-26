//
// Created by gencom on 31/07/17.
//

#ifndef BITSTREAMIO_OUTPUTBITSTREAM_H
#define BITSTREAMIO_OUTPUTBITSTREAM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    unsigned char currentChar;
    uint8_t currentPositionInChar;
    FILE *output;
    uint64_t bitsWritten;
} OutputBitstream;

bool writeBuffer(OutputBitstream *outputBitstream);
bool writeBit(OutputBitstream *outputBitstream, uint8_t value);
bool writeNBitsShift(OutputBitstream *outputBitstream, uint8_t n, const char *value);
bool writeBytes(OutputBitstream *outputBitstream, uint8_t n, const char *value);
bool writeNBitsShiftAndConvertToLittleEndian16(OutputBitstream *outputBitstream, uint8_t n, uint16_t value);
bool writeNBitsShiftAndConvertToLittleEndian32(OutputBitstream *outputBitstream, uint8_t n, uint32_t value);
bool writeNBitsShiftAndConvertToLittleEndian64(OutputBitstream *outputBitstream, uint8_t n, uint64_t value);
bool writeNBitsShiftAndConvertToBigEndian16(OutputBitstream *outputBitstream, uint8_t n, uint16_t value);
bool writeNBitsShiftAndConvertToBigEndian32(OutputBitstream *outputBitstream, uint8_t n, uint32_t value);

bool writeNBitsShiftAndConvertToBigEndian32_new(OutputBitstream *outputBitstream, uint8_t n, uint32_t value);
bool writeNBitsFromByteArray(OutputBitstream *outputBitstream, uint64_t numBits, uint64_t sizeByteArrayInBytes,
                             const uint8_t *values);
bool writeNBitsShiftAndConvertToBigEndian64(OutputBitstream *outputBitstream, uint8_t n, uint64_t value);
bool writeNBitsShiftAndConvertToBigEndian64_new(OutputBitstream *outputBitstream, uint8_t n, uint32_t value);
bool initializeOutputBitstreamFilename(OutputBitstream *outputBitstream, const char *filename);
bool initializeOutputBitstream(OutputBitstream *outputBitstream, FILE *output);
bool writeToBitstream(OutputBitstream *outputBitstream, uint8_t value);
bool writeLittleEndian16ToBitstream(OutputBitstream *outputBitstream, uint16_t value);
bool writeLittleEndian32ToBitstream(OutputBitstream *outputBitstream, uint32_t value);
bool writeLittleEndian64ToBitstream(OutputBitstream *outputBitstream, uint64_t value);
bool writeBigEndian16ToBitstream(OutputBitstream *outputBitstream, uint16_t value);
bool writeBigEndian32ToBitstream(OutputBitstream *outputBitstream, uint32_t value);
bool writeBigEndian64ToBitstream(OutputBitstream *outputBitstream, uint64_t value);

#endif  // BITSTREAMIO_OUTPUTBITSTREAM_H

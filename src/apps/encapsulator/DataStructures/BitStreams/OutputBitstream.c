//
// Created by gencom on 31/07/17.
//

#include "OutputBitstream.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils.h"

bool writeNBits(OutputBitstream *outputBitstream, uint8_t n, const char *value);

bool writeBuffer(OutputBitstream *outputBitstream) {
    if (outputBitstream == NULL) {
        fprintf(stderr, "outputBitstream is null\n");
        return false;
    }
    if (outputBitstream->currentPositionInChar != 0) {
        if (outputBitstream->output == NULL) {
            fprintf(stderr, "output is null\n");
        }
        if (&(outputBitstream->currentChar) == NULL) {
            fprintf(stderr, "current char address is null\n");
        }
        if (fwrite(&(outputBitstream->currentChar), 1, 1, outputBitstream->output) != 1) {
            return false;
        }
        fflush(outputBitstream->output);
        outputBitstream->currentPositionInChar = 0;
        outputBitstream->currentChar = 0;
    }

    return true;
}

bool writeBit(OutputBitstream *outputBitstream, uint8_t value) {
    if (outputBitstream->currentPositionInChar == 8) {
        if (!writeBuffer(outputBitstream)) {
            return false;
        }
    }
    if (value) {
        outputBitstream->currentChar |= (uint8_t)1 << (7 - outputBitstream->currentPositionInChar);
    } else {
        outputBitstream->currentChar &= ~((uint8_t)1 << (7 - outputBitstream->currentPositionInChar));
    }
    outputBitstream->currentPositionInChar++;
    outputBitstream->bitsWritten++;
    return true;
}

bool writeNBitsShift(OutputBitstream *outputBitstream, uint8_t n, const char *value) {
    if (n == 0) {
        return true;
    }
    uint64_t numberBytes = bitsToBytes(n);
    uint8_t *buffer = (uint8_t *)malloc(numberBytes * sizeof(numberBytes));
    memcpy(buffer, value, numberBytes);
    shiftLeft(buffer, 1, (uint8_t)8 - n);
    bool result = writeNBits(outputBitstream, n, (char *)buffer);
    free(buffer);
    return result;
}

bool writeBytes(OutputBitstream *outputBitstream, uint8_t n, const char *value) {
    return writeNBits(outputBitstream, (uint8_t)(n * 8), value);
}

bool writeNBits(OutputBitstream *outputBitstream, uint8_t n, const char *value) {
    int current_byte = 0;
    int current_bit = 0;

    for (int i = 0; i < n; i++) {
        bool valueToWrite = ((value[current_byte] << (current_bit)) & 128) != 0;
        if (!writeBit(outputBitstream, (uint8_t)valueToWrite)) {
            return false;
        }
        current_bit++;
        if (current_bit == 8) {
            current_byte++;
            current_bit = 0;
        }
    }
    return true;
}

bool writeNBitsShiftAndConvertToLittleEndian16(OutputBitstream *outputBitstream, uint8_t n, uint16_t value) {
    uint16_t buffer = nativeToLittleEndian16(value);
    buffer <<= (16 - n);
    return writeNBits(outputBitstream, n, (char *)&buffer);
}
bool writeNBitsShiftAndConvertToLittleEndian32(OutputBitstream *outputBitstream, uint8_t n, uint32_t value) {
    uint32_t buffer = nativeToLittleEndian32(value);
    buffer <<= (32 - n);
    return writeNBits(outputBitstream, n, (char *)&buffer);
}
bool writeNBitsShiftAndConvertToLittleEndian64(OutputBitstream *outputBitstream, uint8_t n, uint64_t value) {
    uint64_t buffer = nativeToLittleEndian64(value);
    buffer <<= (64 - n);
    return writeNBits(outputBitstream, n, (char *)&buffer);
}

bool writeNBitsShiftAndConvertToBigEndian16(OutputBitstream *outputBitstream, uint8_t n, uint16_t value) {
    uint16_t buffer = nativeToBigEndian16(value);

    uint8_t invalid_bits = (uint8_t)16 - n;
    if (invalid_bits <= 8) {
        writeNBitsShift(outputBitstream, (uint8_t)8 - invalid_bits, (char *)(&buffer));
        writeNBits(outputBitstream, 8, (char *)&buffer + 1);
    } else {
        writeNBitsShift(outputBitstream, (uint8_t)16 - invalid_bits, (char *)(&buffer) + 1);
    }
    return true;
}

bool writeNBitsShiftAndConvertToBigEndian32(OutputBitstream *outputBitstream, uint8_t n, uint32_t value) {
    uint32_t buffer = nativeToBigEndian32(value);

    buffer <<= (32 - n);
    return writeNBits(outputBitstream, n, (char *)&buffer);
}

bool writeNBitsShiftAndConvertToBigEndian32_new(OutputBitstream *outputBitstream, uint8_t n, uint32_t value) {
    uint32_t buffer = nativeToBigEndian32(value);

    uint8_t invalid_bits = (uint8_t)32 - n;
    if (invalid_bits <= 8) {
        writeNBitsShift(outputBitstream, (uint8_t)8 - invalid_bits, (char *)(&buffer));
        writeNBits(outputBitstream, 24, (char *)&buffer + 1);
    } else if (invalid_bits <= 16) {
        writeNBitsShift(outputBitstream, (uint8_t)16 - invalid_bits, (char *)(&buffer) + 1);
        writeNBits(outputBitstream, 16, (char *)&buffer + 2);
    } else if (invalid_bits <= 24) {
        writeNBitsShift(outputBitstream, (uint8_t)24 - invalid_bits, (char *)(&buffer) + 2);
        writeNBits(outputBitstream, 8, (char *)&buffer + 3);
    } else {
        writeNBitsShift(outputBitstream, (uint8_t)32 - invalid_bits, (char *)(&buffer) + 3);
    }
    return true;
}

bool writeNBitsFromByteArray(OutputBitstream *outputBitstream, uint64_t numBits, uint64_t sizeByteArrayInBytes,
                             const uint8_t *values) {
    uint64_t currentByte = sizeByteArrayInBytes - bitsToBytes(numBits);
    uint64_t bitsLeftToWrite = numBits;

    while (bitsLeftToWrite > 0) {
        if (bitsLeftToWrite % 8 != 0) {
            if (!writeNBitsShift(outputBitstream, (uint8_t)(bitsLeftToWrite % 8), (const char *)values + currentByte)) {
                return false;
            };
            bitsLeftToWrite -= bitsLeftToWrite % 8;
        } else {
            if (!writeNBits(outputBitstream, 8, (const char *)values + currentByte)) {
                return false;
            };
            bitsLeftToWrite -= 8;
        }
        currentByte++;
    }
    return true;
}

bool writeNBitsShiftAndConvertToBigEndian64(OutputBitstream *outputBitstream, uint8_t n, uint64_t value) {
    uint64_t buffer = nativeToBigEndian64(value);
    buffer <<= (64 - n);
    return writeNBits(outputBitstream, n, (char *)&buffer);
}

bool writeNBitsShiftAndConvertToBigEndian64_new(OutputBitstream *outputBitstream, uint8_t n, uint32_t value) {
    uint64_t buffer = nativeToBigEndian64(value);

    uint8_t invalid_bits = (uint8_t)64 - n;
    if (invalid_bits <= 8) {
        writeNBitsShift(outputBitstream, (uint8_t)8 - invalid_bits, (char *)(&buffer) + 0);
        writeNBits(outputBitstream, 56, (char *)&buffer + 1);
    } else if (invalid_bits <= 16) {
        writeNBitsShift(outputBitstream, (uint8_t)16 - invalid_bits, (char *)(&buffer) + 1);
        writeNBits(outputBitstream, 48, (char *)&buffer + 2);
    } else if (invalid_bits <= 24) {
        writeNBitsShift(outputBitstream, (uint8_t)24 - invalid_bits, (char *)(&buffer) + 2);
        writeNBits(outputBitstream, 40, (char *)&buffer + 3);
    } else if (invalid_bits <= 32) {
        writeNBitsShift(outputBitstream, (uint8_t)32 - invalid_bits, (char *)(&buffer) + 3);
        writeNBits(outputBitstream, 32, (char *)&buffer + 4);
    } else if (invalid_bits <= 40) {
        writeNBitsShift(outputBitstream, (uint8_t)40 - invalid_bits, (char *)(&buffer) + 4);
        writeNBits(outputBitstream, 24, (char *)&buffer + 5);
    } else if (invalid_bits <= 48) {
        writeNBitsShift(outputBitstream, (uint8_t)48 - invalid_bits, (char *)(&buffer) + 5);
        writeNBits(outputBitstream, 24, (char *)&buffer + 6);
    } else if (invalid_bits <= 56) {
        writeNBitsShift(outputBitstream, (uint8_t)56 - invalid_bits, (char *)(&buffer) + 6);
        writeNBits(outputBitstream, 24, (char *)&buffer + 7);
    } else {
        writeNBitsShift(outputBitstream, (uint8_t)64 - invalid_bits, (char *)(&buffer) + 3);
    }
    return true;
}

bool initializeOutputBitstreamFilename(OutputBitstream *outputBitstream, const char *filename) {
    outputBitstream->currentPositionInChar = 0;
    outputBitstream->currentChar = 0;
    outputBitstream->bitsWritten = 0;
    outputBitstream->output = fopen(filename, "wb");
    if (outputBitstream->output == NULL) {
        printf("Error %s \n", strerror(errno));
    }
    return outputBitstream->output != NULL;
}

bool initializeOutputBitstream(OutputBitstream *outputBitstream, FILE *output) {
    outputBitstream->currentPositionInChar = 0;
    outputBitstream->currentChar = 0;
    outputBitstream->output = output;
    outputBitstream->bitsWritten = 0;
    return outputBitstream->output != NULL;
}

bool writeToBitstream(OutputBitstream *outputBitstream, uint8_t value) {
    return writeNBits(outputBitstream, 8, (char *)&value);
}

bool writeLittleEndian16ToBitstream(OutputBitstream *outputBitstream, uint16_t value) {
    uint16_t valueLittleEndian = nativeToLittleEndian16(value);
    return writeNBits(outputBitstream, 16, (char *)&valueLittleEndian);
}

bool writeLittleEndian32ToBitstream(OutputBitstream *outputBitstream, uint32_t value) {
    uint32_t valueLittleEndian = nativeToLittleEndian32(value);
    return writeNBits(outputBitstream, 32, (char *)&valueLittleEndian);
}

bool writeLittleEndian64ToBitstream(OutputBitstream *outputBitstream, uint64_t value) {
    uint64_t valueLittleEndian = nativeToLittleEndian64(value);
    return writeNBits(outputBitstream, 64, (char *)&valueLittleEndian);
}

bool writeBigEndian16ToBitstream(OutputBitstream *outputBitstream, uint16_t value) {
    uint16_t valueBigEndian = nativeToBigEndian16(value);
    return writeNBits(outputBitstream, 16, (char *)&valueBigEndian);
}

bool writeBigEndian32ToBitstream(OutputBitstream *outputBitstream, uint32_t value) {
    uint32_t valueBigEndian = nativeToBigEndian32(value);
    return writeNBits(outputBitstream, 32, (char *)&valueBigEndian);
}

bool writeBigEndian64ToBitstream(OutputBitstream *outputBitstream, uint64_t value) {
    uint64_t valueBigEndian = nativeToBigEndian64(value);
    return writeNBits(outputBitstream, 64, (char *)&valueBigEndian);
}
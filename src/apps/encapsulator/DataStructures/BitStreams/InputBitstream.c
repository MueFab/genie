//
// Created by gencom on 31/07/17.
//

#include "InputBitstream.h"
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "../../utils.h"

bool readNBits(InputBitstream *inputBitstream, uint32_t n, char *value);

bool forwardUntilAligned(InputBitstream *inputBitstream) {
    uint8_t buffer;
    while (inputBitstream->currentPositionInChar != 0 && inputBitstream->currentPositionInChar != 8) {
        if (!readBit(inputBitstream, &buffer)) {
            return false;
        };
    }
    return true;
}

bool readBit(InputBitstream *inputBitstream, uint8_t *value) {
    if (inputBitstream->currentPositionInChar == 8) {
        uint8_t fileReadValue;
        bool success = fread(&fileReadValue, sizeof(fileReadValue), 1, inputBitstream->input) == 1;
        if (!success) {
            return false;
        }
        inputBitstream->currentPositionInChar = 0;
        inputBitstream->currentChar = fileReadValue;
    }

    uint8_t mask = 1;
    mask <<= (7 - inputBitstream->currentPositionInChar);
    *value = inputBitstream->currentChar & mask;

    inputBitstream->currentPositionInChar++;
    return true;
}

bool readNBits8(InputBitstream *inputBitstream, uint8_t n, uint8_t *value) {
    *value = 0;
    bool result = readBitsToByteArray(inputBitstream, n, 1, (uint8_t *)value);

    if (!result) {
        return result;
    }
    return true;
};

bool readNBitsShift(InputBitstream *inputBitstream, uint32_t n, char *value) {
    if (!readNBits(inputBitstream, n, value)) {
        return false;
    }
    int numberBytes = 1;
    uint8_t numberBits = (uint8_t)8 - (uint8_t)n;
    shiftRight((uint8_t *)value, numberBytes, numberBits);
    return true;
}

bool readBitsToByteArray(InputBitstream *inputBitStream, uint64_t numBits, uint64_t sizeByteArrayInBytes,
                         uint8_t *byteArray) {
    uint64_t remainingBits = numBits;
    uint64_t currentByte = sizeByteArrayInBytes - bitsToBytes(numBits);
    while (remainingBits > 0) {
        uint8_t toRead;
        if (remainingBits % 8 != 0) {
            toRead = (uint8_t)(remainingBits % 8);
            if (!readNBitsShift(inputBitStream, toRead, (char *)byteArray + currentByte)) {
                return false;
            };
        } else {
            toRead = 8;
            if (!readNBits(inputBitStream, toRead, (char *)byteArray + currentByte)) {
                return false;
            };
        }

        remainingBits -= toRead;
        currentByte++;
    }
    return true;
}

bool readBytes(InputBitstream *inputBitstream, uint32_t n, char *value) {
    return readNBits(inputBitstream, n * 8, value);
}

bool readNBits(InputBitstream *inputBitstream, uint32_t n, char *value) {
    uint8_t bufferValue = 0;

    int current_bit = 0;
    int current_byte = 0;

    for (uint32_t i = 0; i < n; i++) {
        if (!readBit(inputBitstream, &bufferValue)) {
            return false;
        }

        if (bufferValue != 0) {
            value[current_byte] |= ((uint8_t)1) << (7 - current_bit);
        } else {
            value[current_byte] &= ~(((uint8_t)1) << (7 - current_bit));
        }
        current_bit++;
        if (current_bit == 8) {
            current_byte++;
            current_bit = 0;
        }
    }
    return true;
}

bool readNBitsBigToNativeEndian16(InputBitstream *inputBitstream, uint8_t n, uint16_t *value) {
    bool result = readBitsToByteArray(inputBitstream, n, 2, (uint8_t *)value);

    if (!result) {
        return result;
    }
    *value = bigToNativeEndian16(*value);
    return true;
}
bool readNBitsBigToNativeEndian32(InputBitstream *inputBitstream, uint8_t n, uint32_t *value) {
    uint32_t buffer = 0;
    bool result = readBitsToByteArray(inputBitstream, n, 4, (uint8_t *)&buffer);
    if (!result) {
        return result;
    }
    buffer = bigToNativeEndian32(buffer);
    memcpy(value, &buffer, 4);
    return true;
}
bool readNBitsBigToNativeEndian64(InputBitstream *inputBitstream, uint8_t n, uint64_t *value) {
    uint64_t buffer = 0;
    bool result = readBitsToByteArray(inputBitstream, n, 8, (uint8_t *)&buffer);
    if (!result) {
        return result;
    }
    buffer = bigToNativeEndian64(buffer);
    memcpy(value, &buffer, 8);
    return true;
}

bool initializeInputBitstreamFromFilename(InputBitstream *inputBitstream, const char *filename) {
    inputBitstream->currentPositionInChar = 8;
    inputBitstream->input = fopen(filename, "rb");
    if (inputBitstream->input == NULL) {
        printf("Error %s \n", strerror(errno));
    }
    return inputBitstream->input != NULL;
}

bool initializeInputBitstream(InputBitstream *inputBitstream, FILE *inputFile) {
    inputBitstream->currentPositionInChar = 8;
    inputBitstream->input = inputFile;
    return inputBitstream->input != NULL;
}

bool eofInputBitstream(InputBitstream *inputBitstream) { return (bool)feof(inputBitstream->input); }
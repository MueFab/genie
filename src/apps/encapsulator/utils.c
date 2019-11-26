//
// Created by gencom on 24/07/17.
//

#include <stdint.h>
#ifdef __APPLE_CC__
#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)
#else
#include <endian.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"

uint8_t generateMask(uint8_t numberBits);

uint16_t nativeToBigEndian16(uint16_t nativeValue) { return htobe16(nativeValue); }

uint32_t nativeToBigEndian32(uint32_t nativeValue) { return htobe32(nativeValue); }

uint64_t nativeToBigEndian64(uint64_t nativeValue) { return htobe64(nativeValue); }

uint16_t nativeToLittleEndian16(uint16_t nativeValue) { return htole16(nativeValue); }

uint32_t nativeToLittleEndian32(uint32_t nativeValue) { return htole32(nativeValue); }

uint64_t nativeToLittleEndian64(uint64_t nativeValue) { return htole64(nativeValue); }

uint64_t bigToNativeEndian64(uint64_t bigValue) { return be64toh(bigValue); }

uint16_t bigToNativeEndian16(uint16_t bigValue) { return be16toh(bigValue); }

uint32_t bigToNativeEndian32(uint32_t bigValue) { return be32toh(bigValue); }

uint16_t littleToNativeEndian16(uint16_t littleValue) { return le16toh(littleValue); }

uint32_t littleToNativeEndian32(uint32_t littleValue) { return le32toh(littleValue); }

uint64_t littleToNativeEndian64(uint64_t littleValue) { return le64toh(littleValue); }

bool checkBitValue(const char *bits, uint64_t byte_i, uint8_t bit_i) {
    char currentByte = bits[byte_i];
    uint8_t mask = (uint8_t)(1 << (7 - bit_i));
    return (currentByte & mask) != 0;
}

bool firstNBitsSetTo(char *bits, uint64_t N, bool value) {
    uint64_t byte_i = 0;
    uint8_t bit_in_byte = 0;
    for (uint64_t bit_i = 0; bit_i < N; bit_i++) {
        if (bit_in_byte == 8) {
            bit_in_byte = 0;
            byte_i++;
        }

        bool read_value = checkBitValue(bits, byte_i, bit_in_byte);
        if (read_value != value) {
            return false;
        }
        bit_in_byte++;
    }
    return true;
}

uint8_t generateMask(uint8_t numberBits) {
    uint8_t mask = 0;
    for (int i = 1; i <= numberBits; i++) {
        mask |= 1 << (i - 1);
    }
    return mask;
}

void shiftLeft(uint8_t *bytes, int numberBytes, uint8_t numberBits) {
    uint8_t lowerMask = generateMask(numberBits);
    uint8_t upperMask = lowerMask << (8 - numberBits);
    uint8_t valueToCarry = 0;
    for (int i = 0; i < numberBytes; i++) {
        uint8_t newValueToCarry = bytes[i] & upperMask;
        newValueToCarry >>= (8 - numberBits);

        bytes[i] <<= numberBits;
        if (i != 0) {
            bytes[i] |= valueToCarry;
        }
        valueToCarry = newValueToCarry;
    }
}

void shiftRight(uint8_t *bytes, int numberBytes, uint8_t numberBits) {
    uint8_t lowerMask = generateMask(numberBits);
    uint8_t valueToCarry = 0;
    for (int i = 0; i < numberBytes; i++) {
        uint8_t newValueToCarry = bytes[i] & lowerMask;
        newValueToCarry <<= (8 - numberBits);

        bytes[i] >>= numberBits;
        if (i != 0) {
            bytes[i] |= valueToCarry;
        }
        valueToCarry = newValueToCarry;
    }
}

bool writeBoxHeader(FILE *output_file, const char *type, uint64_t box_size) {
    size_t size_written_type = fwrite(type, sizeof(char), 4, output_file);
    uint64_t box_size_big = nativeToBigEndian64(box_size);
    size_t size_written_box_size = fwrite(&box_size_big, sizeof(uint64_t), 1, output_file);
    return size_written_type == 4 && size_written_box_size == 1;
}

bool readBoxHeader(FILE *input_file, char *type, uint64_t *boxSize) {
    size_t sizeReadType = fread(type, sizeof(char), 4, input_file);
    uint64_t boxSizeBig;
    size_t sizeReadBoxSize = fread(&boxSizeBig, sizeof(uint64_t), 1, input_file);
    *boxSize = bigToNativeEndian64(boxSizeBig);
    return sizeReadType == 4 && sizeReadBoxSize == 1;
}

bool utils_read(uint8_t *value, FILE *inputFile) {
    int sucRead = fread(value, sizeof(uint8_t), 1, inputFile) == 1;
    fprintf(stderr, "Position in Filestream:%li\n", ftell(inputFile));
    fprintf(stderr, "value is:%u\n", *value);
    if (*value != 1 && *value != 2) {
        fprintf(stderr, "Next 3 values are:\n");
        fread(value, sizeof(uint8_t), 1, inputFile) == 1;
        fprintf(stderr, "value is:%u\n", *value);
        fread(value, sizeof(uint8_t), 1, inputFile) == 1;
        fprintf(stderr, "value is:%u\n", *value);
        fread(value, sizeof(uint8_t), 1, inputFile) == 1;
        fprintf(stderr, "value is:%u\n", *value);
    }
    return sucRead;
    // return fread(value,sizeof(uint8_t),1,inputFile)==1;
}

bool readLittleEndian16FromFile(uint16_t *value, FILE *inputFile) {
    uint16_t valueLittleEndian;
    bool readSuccess = fread(&valueLittleEndian, sizeof(valueLittleEndian), 1, inputFile) == 1;
    *value = littleToNativeEndian16(valueLittleEndian);
    return readSuccess;
}

bool readLittleEndian32FromFile(uint32_t *value, FILE *inputFile) {
    uint32_t valueLittleEndian;
    bool readSuccess = fread(&valueLittleEndian, sizeof(valueLittleEndian), 1, inputFile) == 1;
    *value = littleToNativeEndian32(valueLittleEndian);
    return readSuccess;
}

bool readLittleEndian64FromFile(uint64_t *value, FILE *inputFile) {
    uint64_t valueLittleEndian;
    bool readSuccess = fread(&valueLittleEndian, sizeof(valueLittleEndian), 1, inputFile) == 1;
    *value = littleToNativeEndian64(valueLittleEndian);
    return readSuccess;
}

bool readBigEndian16FromFile(uint16_t *value, FILE *inputFile) {
    uint16_t valueBigEndian;
    bool readSuccess = fread(&valueBigEndian, sizeof(valueBigEndian), 1, inputFile) == 1;
    *value = bigToNativeEndian16(valueBigEndian);
    return readSuccess;
}

bool readBigEndian32FromFile(uint32_t *value, FILE *inputFile) {
    uint32_t valueBigEndian;
    bool readSuccess = fread(&valueBigEndian, sizeof(valueBigEndian), 1, inputFile) == 1;
    *value = bigToNativeEndian32(valueBigEndian);
    return readSuccess;
}

bool readBigEndian64FromFile(uint64_t *value, FILE *inputFile) {
    uint64_t valueBigEndian;
    bool readSuccess = fread(&valueBigEndian, sizeof(valueBigEndian), 1, inputFile) == 1;
    *value = bigToNativeEndian64(valueBigEndian);
    return readSuccess;
}

bool readChars(char *charArray, uint32_t charArrayLength, FILE *inputFile) {
    return fread(charArray, sizeof(char), charArrayLength, inputFile) == charArrayLength;
}

bool utils_write(uint8_t value, FILE *outputFile) { return fwrite(&value, sizeof(value), 1, outputFile) == 1; }

bool writeLittleEndian16ToFile(uint16_t value, FILE *outputFile) {
    uint16_t valueLittleEndian = nativeToLittleEndian16(value);
    size_t valueWrittenSize = fwrite(&valueLittleEndian, sizeof(valueLittleEndian), 1, outputFile);
    return valueWrittenSize == 1;
}

bool writeLittleEndian32ToFile(uint32_t value, FILE *outputFile) {
    uint32_t valueLittleEndian = nativeToLittleEndian32(value);
    size_t valueWrittenSize = fwrite(&valueLittleEndian, sizeof(valueLittleEndian), 1, outputFile);
    return valueWrittenSize == 1;
}

bool writeLittleEndian64ToFile(uint64_t value, FILE *outputFile) {
    uint64_t valueLittleEndian = nativeToLittleEndian64(value);
    size_t valueWrittenSize = fwrite(&valueLittleEndian, sizeof(valueLittleEndian), 1, outputFile);
    return valueWrittenSize == 1;
}

bool writeBigEndian16ToFile(uint16_t value, FILE *outputFile) {
    uint16_t valueBigEndian = nativeToBigEndian16(value);
    size_t valueWrittenSize = fwrite(&valueBigEndian, sizeof(valueBigEndian), 1, outputFile);
    return valueWrittenSize == 1;
}

bool writeBigEndian32ToFile(uint32_t value, FILE *outputFile) {
    uint32_t valueBigEndian = nativeToBigEndian32(value);
    size_t valueWrittenSize = fwrite(&valueBigEndian, sizeof(valueBigEndian), 1, outputFile);
    return valueWrittenSize == 1;
}

bool writeBigEndian64ToFile(uint64_t value, FILE *outputFile) {
    uint64_t valueBigEndian = nativeToBigEndian64(value);
    size_t valueWrittenSize = fwrite(&valueBigEndian, sizeof(valueBigEndian), 1, outputFile);
    return valueWrittenSize == 1;
}

uint64_t bitsToBytes(uint64_t numberBits) {
    if (numberBits == 0) return 0;
    return 1 + ((numberBits - 1) / 8);
}

//
// Created by gencom on 24/07/17.
//

#ifndef MPEGG_CAPSULATOR_UTILS_H
#define MPEGG_CAPSULATOR_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

uint64_t bitsToBytes(uint64_t numberBits);

uint16_t nativeToBigEndian16(uint16_t nativeValue);
uint32_t nativeToBigEndian32(uint32_t nativeValue);
uint64_t nativeToBigEndian64(uint64_t nativeValue);
uint16_t nativeToLittleEndian16(uint16_t nativeValue);
uint32_t nativeToLittleEndian32(uint32_t nativeValue);
uint64_t nativeToLittleEndian64(uint64_t nativeValue);
uint16_t bigToNativeEndian16(uint16_t bigValue);
uint32_t bigToNativeEndian32(uint32_t bigValue);
uint64_t bigToNativeEndian64(uint64_t bigValue);
uint16_t littleToNativeEndian16(uint16_t littleValue);
uint32_t littleToNativeEndian32(uint32_t littleValue);
uint64_t littleToNativeEndian64(uint64_t littleValue);

bool checkBitValue(const char *bits, uint64_t byte_i, uint8_t bit_i);
bool firstNBitsSetTo(char *bits, uint64_t N, bool value);

void shiftLeft(uint8_t *bytes, int numberBytes, uint8_t numberBits);
void shiftRight(uint8_t *bytes, int numberBytes, uint8_t numberBits);

bool utils_read(uint8_t *value, FILE *inputFile);
bool readLittleEndian16FromFile(uint16_t *value, FILE *inputFile);
bool readLittleEndian32FromFile(uint32_t *value, FILE *inputFile);
bool readLittleEndian64FromFile(uint64_t *value, FILE *inputFile);
bool readBigEndian16FromFile(uint16_t *value, FILE *inputFile);
bool readBigEndian32FromFile(uint32_t *value, FILE *inputFile);
bool readBigEndian64FromFile(uint64_t *value, FILE *inputFile);
bool readChars(char *charArray, uint32_t charArrayLength, FILE *inputFile);

bool utils_write(uint8_t, FILE *outputFile);
bool writeLittleEndian16ToFile(uint16_t value, FILE *outputFile);
bool writeLittleEndian32ToFile(uint32_t value, FILE *outputFile);
bool writeLittleEndian64ToFile(uint64_t value, FILE *outputFile);
bool writeBigEndian16ToFile(uint16_t value, FILE *outputFile);
bool writeBigEndian32ToFile(uint32_t value, FILE *outputFile);
bool writeBigEndian64ToFile(uint64_t value, FILE *outputFile);

bool writeBoxHeader(FILE *output_file, const char *type, uint64_t box_size);
bool readBoxHeader(FILE *input_file, char *type, uint64_t *boxSize);

#endif  // MPEGG_CAPSULATOR_UTILS_H

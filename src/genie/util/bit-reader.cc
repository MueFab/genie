/**
 * @file bit-reader.cc
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Implementation of the BitReader utility class for reading bits from a stream.
 *
 * This file contains the implementation of the BitReader class, which provides methods
 * to read bits and bytes from an input stream. It supports both non-aligned and byte-aligned
 * reading operations.
 *
 * @details The BitReader class offers functionalities like reading a specific number of bits,
 * flushing bits, checking byte alignment, reading bytes, reading and skipping aligned strings,
 * and manipulating the input stream position.
 */

#include "genie/util/bit-reader.h"
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

constexpr uint8_t BITS_PER_BYTE = 8u;
constexpr uint8_t BYTE_MASK = 0xffu;

// ---------------------------------------------------------------------------------------------------------------------

BitReader::BitReader(std::istream &_istream) : inputStream(_istream), heldBits(0), numHeldBits(0), totalBitsRead(0) {}

// Non Byte-Aligned reading --------------------------------------------------------------------------------------------

inline uint64_t BitReader::loadAlignedWord(const uint8_t numBits) const {
    // NOTE: This unrolling is the result of profiling, do not change.
    uint64_t alignedWord = 0;
    const auto numBitsMinusOne = static_cast<uint8_t>(numBits - 1);
    const auto numBytesToLoad = static_cast<uint8_t>((numBitsMinusOne >> 3) + 1);

    if (numBytesToLoad == 1) goto L1;
    if (numBytesToLoad == 2) goto L2;
    if (numBytesToLoad == 3) goto L3;
    if (numBytesToLoad == 4) goto L4;
    if (numBytesToLoad == 5) goto L5;
    if (numBytesToLoad == 6) goto L6;
    if (numBytesToLoad == 7) goto L7;
    if (numBytesToLoad != 8) goto L0;
    alignedWord |= readAlignedByte() << BITS_PER_BYTE * 7;
L7:
    alignedWord |= readAlignedByte() << BITS_PER_BYTE * 6;
L6:
    alignedWord |= readAlignedByte() << BITS_PER_BYTE * 5;
L5:
    alignedWord |= readAlignedByte() << BITS_PER_BYTE * 4;
L4:
    alignedWord |= readAlignedByte() << BITS_PER_BYTE * 3;
L3:
    alignedWord |= readAlignedByte() << BITS_PER_BYTE * 2;
L2:
    alignedWord |= readAlignedByte() << BITS_PER_BYTE;
L1:
    alignedWord |= readAlignedByte();
L0:
    return alignedWord;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::readBits(uint8_t numBits) {
    totalBitsRead += numBits;
    uint64_t resultBits;
    if (numBits <= numHeldBits) {
        resultBits = heldBits >> static_cast<uint8_t>(numHeldBits - numBits);
        resultBits &= ~(static_cast<uint64_t>(BYTE_MASK) << numBits);
        numHeldBits -= numBits;
        return resultBits;
    }

    numBits -= numHeldBits;
    resultBits = heldBits & ~(BYTE_MASK << numHeldBits);
    resultBits <<= numBits;

    const uint64_t alignedWord = loadAlignedWord(numBits);
    const auto numNextHeldBits = static_cast<uint8_t>((BITS_PER_BYTE * 8 - numBits) % BITS_PER_BYTE);
    resultBits |= alignedWord >> numNextHeldBits;

    numHeldBits = numNextHeldBits;
    heldBits = static_cast<uint8_t>(alignedWord & BYTE_MASK);
    return resultBits;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::getTotalBitsRead() const { return totalBitsRead; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::flushHeldBits() {
    totalBitsRead += numHeldBits;
    numHeldBits = 0;
    const auto ret = heldBits;
    heldBits = 0;
    return ret;
}

// Byte-Aligned Reading ------------------------------------------------------------------------------------------------

bool BitReader::isByteAligned() const { return !numHeldBits; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::readAlignedByte() const {
    char c = 0;
    inputStream.read(&c, 1);
    return static_cast<uint8_t>(c);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string BitReader::readAlignedStringTerminated() {
    std::string result;
    char c;
    while ((c = readAlignedInt<char>()) != '\0') {
        result.push_back(c);
    }
    totalBitsRead += result.length() * BITS_PER_BYTE;
    return result;
}

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::readAlignedBytes(void *in, const size_t size) {
    totalBitsRead += size * BITS_PER_BYTE;
    inputStream.read(static_cast<char *>(in), static_cast<std::streamsize>(size));
}

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::skipAlignedBytes(const size_t bytes) const {
    inputStream.seekg(static_cast<std::istream::off_type>(bytes), std::ios_base::cur);
}

// Input stream manipulation -------------------------------------------------------------------------------------------

int64_t BitReader::getStreamPosition() const { return inputStream.tellg(); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::setStreamPosition(const int64_t pos) const { inputStream.seekg(pos, std::ios_base::beg); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::clearStreamState() const { inputStream.clear(); }

// ---------------------------------------------------------------------------------------------------------------------

bool BitReader::isStreamGood() const { return static_cast<bool>(inputStream); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

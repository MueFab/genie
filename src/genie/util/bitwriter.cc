/**
 * @file bitwriter.cc
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/mitogen/genie for more details.
 *
 * @brief Implementation of the BitWriter utility class for writing bits to a stream.
 *
 * This file contains the implementation of the BitWriter class, which provides methods
 * to write bits and bytes to an output stream. It supports both non-aligned and byte-aligned
 * writing operations.
 *
 * @details The BitWriter class offers functionalities like writing a specific number of bits,
 * flushing bits, checking byte alignment, writing bytes, writing aligned streams, and manipulating
 * the output stream position. The class ensures efficient bit-level writing, while also handling
 * any buffered bits that cannot yet be written out as full bytes.
 */

#include "genie/util/bitwriter.h"
#include <string>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

constexpr uint8_t BITS_PER_BYTE = 8u;
constexpr uint8_t BYTE_MASK = 0xffu;

// ---------------------------------------------------------------------------------------------------------------------

BitWriter::BitWriter(std::ostream &str) : stream(str), heldBits(0), numHeldBits(0), totalBitsWritten(0) {}

// ---------------------------------------------------------------------------------------------------------------------

BitWriter::~BitWriter() { flushBits(); }

// ---------------------------------------------------------------------------------------------------------------------

inline void BitWriter::writeAlignedBytes(uint64_t writeBits, uint8_t numBytes) {
    if (numBytes == 1) goto L1;
    if (numBytes == 2) goto L2;
    if (numBytes == 3) goto L3;
    if (numBytes == 4) goto L4;
    if (numBytes == 5) goto L5;
    if (numBytes == 6) goto L6;
    if (numBytes == 7) goto L7;
    if (numBytes != 8) goto L0;
    writeAlignedByte(static_cast<uint8_t>((writeBits >> BITS_PER_BYTE * 7) & BYTE_MASK));
L7:
    writeAlignedByte(static_cast<uint8_t>((writeBits >> BITS_PER_BYTE * 6) & BYTE_MASK));
L6:
    writeAlignedByte(static_cast<uint8_t>((writeBits >> BITS_PER_BYTE * 5) & BYTE_MASK));
L5:
    writeAlignedByte(static_cast<uint8_t>((writeBits >> BITS_PER_BYTE * 4) & BYTE_MASK));
L4:
    writeAlignedByte(static_cast<uint8_t>((writeBits >> BITS_PER_BYTE * 3) & BYTE_MASK));
L3:
    writeAlignedByte(static_cast<uint8_t>((writeBits >> BITS_PER_BYTE * 2) & BYTE_MASK));
L2:
    writeAlignedByte(static_cast<uint8_t>((writeBits >> BITS_PER_BYTE) & BYTE_MASK));
L1:
    writeAlignedByte(static_cast<uint8_t>(writeBits & BYTE_MASK));
L0:
    return;
}

void BitWriter::writeBits(uint64_t value, uint8_t bits) {
    uint8_t numTotalBits = bits + numHeldBits;
    auto nextHeldBitCount = uint8_t(numTotalBits % BITS_PER_BYTE);
    auto newHeldBits = static_cast<uint8_t>((value << (BITS_PER_BYTE - nextHeldBitCount)) & BYTE_MASK);

    if (numTotalBits < BITS_PER_BYTE) {
        heldBits |= newHeldBits;
        numHeldBits = nextHeldBitCount;
        return;
    }

    uint64_t topword = uint64_t(bits - nextHeldBitCount) & uint64_t(~((1u << 3u) - 1u));
    uint64_t writeBits = (heldBits << topword) | (value >> nextHeldBitCount);

    uint8_t numBytesToWrite = numTotalBits / BITS_PER_BYTE;
    writeAlignedBytes(writeBits, numBytesToWrite);

    heldBits = newHeldBits;
    numHeldBits = nextHeldBitCount;
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::flushBits() {
    if (numHeldBits == 0) {
        return;
    }
    writeBits(heldBits, uint8_t(BITS_PER_BYTE - numHeldBits));
    heldBits = 0x00;
    numHeldBits = 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitWriter::getTotalBitsWritten() const { return totalBitsWritten + numHeldBits; }

// ---------------------------------------------------------------------------------------------------------------------

bool BitWriter::isByteAligned() const { return numHeldBits == 0; }

// ---------------------------------------------------------------------------------------------------------------------

inline void BitWriter::writeAlignedByte(uint8_t byte) {
    stream.write(reinterpret_cast<char *>(&byte), 1);
    totalBitsWritten += BITS_PER_BYTE;
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::writeAlignedBytes(const void *in, size_t size) {
    this->totalBitsWritten += size * BITS_PER_BYTE;
    UTILS_DIE_IF(!isByteAligned(), "Writer not aligned when it should be");
    stream.write(reinterpret_cast<const char *>(in), size);
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::writeAlignedStream(std::istream &in) {
    UTILS_DIE_IF(!isByteAligned(), "Writer not aligned when it should be");
    const size_t BUFFERSIZE = 100;
    char byte[BUFFERSIZE];
    do {
        in.read(byte, BUFFERSIZE);
        stream.write(byte, in.gcount());
        this->totalBitsWritten += in.gcount() * BITS_PER_BYTE;
    } while (in.gcount() == BUFFERSIZE);
}

// ---------------------------------------------------------------------------------------------------------------------

int64_t BitWriter::getStreamPosition() const { return stream.tellp(); }

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::setStreamPosition(int64_t pos) { stream.seekp(pos, std::ios::beg); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

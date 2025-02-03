/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/bit_reader.h"
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::GetTotalBitsRead() const { return bitsRead; }

// ---------------------------------------------------------------------------------------------------------------------

bool BitReader::IsByteAligned() const { return !m_numHeldBits; }

// ---------------------------------------------------------------------------------------------------------------------

int64_t BitReader::GetStreamPosition() const { return istream.tellg(); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::SetStreamPosition(int64_t pos) const { istream.seekg(pos, std::ios_base::beg); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::ClearStreamState() { istream.clear(); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::SkipAlignedBytes(size_t bytes) { istream.seekg(bytes, std::ios_base::cur); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::ReadAlignedBytes(void *in, size_t size) {
    bitsRead += size * 8;
    istream.read(reinterpret_cast<char *>(in), size);
}

// ---------------------------------------------------------------------------------------------------------------------

BitReader::BitReader(std::istream &_istream) : istream(_istream), m_heldBits(0), m_numHeldBits(0), bitsRead(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::ReadAlignedByte() {
    char c = 0;
    istream.read(&c, 1);
    return uint8_t(c);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::FlushHeldBits() {
    auto ret = m_heldBits;
    m_heldBits = 0;
    bitsRead += m_numHeldBits;
    m_numHeldBits = 0;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::ReadBits(uint8_t numBits) {
    bitsRead += numBits;
    uint64_t bits;
    if (numBits <= m_numHeldBits) {
        // Get numBits most significant bits from heldBits as bits
        bits = m_heldBits >> uint8_t(m_numHeldBits - numBits);
        bits &= ~(uint64_t(0xff) << numBits);
        m_numHeldBits -= numBits;
        return bits;
    }

    // More bits requested than currently held, FlushHeldBits all heldBits to bits
    numBits -= m_numHeldBits;
    bits = m_heldBits & ~(0xffu << m_numHeldBits);
    // bits = static_cast<uint64_t>(m_heldBits & ~(0xffu << m_numHeldBits));
    bits <<= numBits;  // make room for the bits to come

    // Read in more bytes to satisfy the request
    auto numBytesToLoad = uint8_t(((numBits - 1u) >> 3u) + 1);
    uint64_t alignedWord = 0;
    // uint64_t alignedWord = 0;
    if (numBytesToLoad == 1) {
        goto L1;
    } else if (numBytesToLoad == 2) {
        goto L2;
    } else if (numBytesToLoad == 3) {
        goto L3;
    } else if (numBytesToLoad == 4) {
        goto L4;
    } else if (numBytesToLoad == 5) {
        goto L5;
    } else if (numBytesToLoad == 6) {
        goto L6;
    } else if (numBytesToLoad == 7) {
        goto L7;
    } else if (numBytesToLoad != 8) {
        goto L0;
    }

    alignedWord |= (ReadAlignedByte() << 56u);
L7:
    alignedWord |= (ReadAlignedByte() << 48u);
L6:
    alignedWord |= (ReadAlignedByte() << 40u);
L5:
    alignedWord |= (ReadAlignedByte() << 32u);
L4:
    alignedWord |= (ReadAlignedByte() << 24u);
L3:
    alignedWord |= (ReadAlignedByte() << 16u);
L2:
    alignedWord |= (ReadAlignedByte() << 8u);
L1:
    alignedWord |= (ReadAlignedByte());
L0:

    // Resolve remainder bits
    auto numNextHeldBits = uint8_t((64 - numBits) % 8);

    // Copy required part of alignedWord into bits
    bits |= alignedWord >> numNextHeldBits;

    // Store held bits
    m_numHeldBits = numNextHeldBits;
    /* FIXME @Jan: Can you please check, why this assert is needed?
     * I checked in ref-soft file mpegg-stream.c (in mpegg_input_bitstream_read_ubits(...)),
     * and did not see any such assert.
     */
    m_heldBits = static_cast<uint8_t>(alignedWord & 0xffu);

    return bits;
}

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::readBypass(std::string &str) {
    bitsRead += str.length() * 8;
    istream.read(&str[0], str.length());
}

// ---------------------------------------------------------------------------------------------------------------------

bool BitReader::IsStreamGood() const { return static_cast<bool>(istream); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::ReadAlignedStringTerminated(std::string &str) {
    str.clear();
    auto c = ReadAlignedInt<char>();
    while (c != 0) {
        str.push_back(c);
        c = ReadAlignedInt<char>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

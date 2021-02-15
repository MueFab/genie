/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/bitreader.h"
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::getBitsRead() const { return bitsRead; }

// ---------------------------------------------------------------------------------------------------------------------

bool BitReader::isAligned() const { return !m_numHeldBits; }

// ---------------------------------------------------------------------------------------------------------------------

int64_t BitReader::getPos() const { return istream.tellg(); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::setPos(int64_t pos) const { istream.seekg(pos, std::ios_base::beg); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::clear() { istream.clear(); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::skip(size_t bytes) { istream.seekg(bytes, std::ios_base::cur); }

// ---------------------------------------------------------------------------------------------------------------------

void BitReader::readBypass(void *in, size_t size) {
    bitsRead += size * 8;
    istream.read(reinterpret_cast<char *>(in), size);
}

// ---------------------------------------------------------------------------------------------------------------------

BitReader::BitReader(std::istream &_istream) : istream(_istream), m_heldBits(0), m_numHeldBits(0), bitsRead(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::getByte() {
    char c = 0;
    istream.read(&c, 1);
    return uint8_t(c);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::flush() {
    auto ret = m_heldBits;
    m_heldBits = 0;
    bitsRead += m_numHeldBits;
    m_numHeldBits = 0;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::read_b(uint8_t numBits) {
    bitsRead += numBits;
    uint64_t bits;
    if (numBits <= m_numHeldBits) {
        // Get numBits most significant bits from heldBits as bits
        bits = m_heldBits >> uint8_t(m_numHeldBits - numBits);
        bits &= ~(uint64_t(0xff) << numBits);
        m_numHeldBits -= numBits;
        return bits;
    }

    // More bits requested than currently held, flush all heldBits to bits
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

    alignedWord |= (getByte() << 56u);
L7:
    alignedWord |= (getByte() << 48u);
L6:
    alignedWord |= (getByte() << 40u);
L5:
    alignedWord |= (getByte() << 32u);
L4:
    alignedWord |= (getByte() << 24u);
L3:
    alignedWord |= (getByte() << 16u);
L2:
    alignedWord |= (getByte() << 8u);
L1:
    alignedWord |= (getByte());
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

bool BitReader::isGood() const { return static_cast<bool>(istream); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

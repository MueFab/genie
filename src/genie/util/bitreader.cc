/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

BitReader::BitReader(std::istream &_istream) : istream(_istream), m_heldBits(0), m_numHeldBits(0) {}

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
    m_numHeldBits = 0;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitReader::read(uint8_t numBits) {
    uint64_t bits = 0;
    // uint64_t bits = 0;
    if (numBits <= m_numHeldBits) {
        // Get numBits most significant bits from heldBits as bits
        bits = m_heldBits >> (m_numHeldBits - numBits);
        bits &= ~(0xffu << numBits);
        m_numHeldBits -= numBits;
        return bits;
    }

    // More bits requested than currently held, flush all heldBits to bits
    numBits -= m_numHeldBits;
    bits = m_heldBits & ~(0xffu << m_numHeldBits);
    // bits = static_cast<uint64_t>(m_heldBits & ~(0xffu << m_numHeldBits));
    bits <<= numBits;  // make room for the bits to come

    // Read in more bytes to satisfy the request
    uint8_t numBytesToLoad = ((numBits - 1u) >> 3u) + 1;
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
    uint8_t numNextHeldBits = (64 - numBits) % 8;

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

void BitReader::read(std::string &str) {
    for (auto &c : str) {
        c = char(read(8));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool BitReader::isGood() const { return bool(istream); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
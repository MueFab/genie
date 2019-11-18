/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "bit-input-stream.h"
#include <cassert>
#include <limits>
#include "data-block.h"

namespace gabac {

inline static unsigned char readIn(gabac::BlockStepper *reader) {
    if (!reader->isValid()) {
        GABAC_DIE("Index out of bounds");
    }
    // TODO(Jan): We here rely on that get() returns exactly 1 byte. However, it might happen that it returns multiple
    // bytes. Fix that.
    auto byte = static_cast<unsigned char>(reader->get());
    reader->inc();
    return byte;
}

BitInputStream::BitInputStream(DataBlock *const bitstream) : m_bitstream(bitstream), m_heldBits(0), m_numHeldBits(0) {
    m_reader = m_bitstream->getReader();  // TODO(Jan): This line is redundant.
    reset();
}

BitInputStream::~BitInputStream() = default;

unsigned int BitInputStream::getNumBitsUntilByteAligned() const { return m_numHeldBits & 0x7u; }

unsigned char BitInputStream::readByte() {
    unsigned int result = read(8);
    assert(result <= std::numeric_limits<unsigned char>::max());
    return static_cast<unsigned char>(result);
}

void BitInputStream::reset() {
    m_heldBits = 0;
    m_numHeldBits = 0;
    m_reader = m_bitstream->getReader();
}

unsigned int BitInputStream::read(unsigned int numBits) {
    // uint64_t BitInputStream::read(const unsigned int numBits) {
    // TODO(Jan): Make this 64-bit compatible. See also:
    //   mpegg-reference-sw/src/gm-common-v1/mpegg-stream.c:mpegg_input_bitstream_read_ubits()
    assert(numBits <= 32);
    // assert(numBits <= 64);

    // Bits are extracted from the MSB of each byte
    unsigned int bits = 0;
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
    unsigned int numBytesToLoad = ((numBits - 1u) >> 3u) + 1;
    unsigned int alignedWord = 0;
    // uint64_t alignedWord = 0;
    if (numBytesToLoad == 1) {
        goto L1;
    } else if (numBytesToLoad == 2) {
        goto L2;
    } else if (numBytesToLoad == 3) {
        goto L3;
    } else if (numBytesToLoad != 4) {
        goto L0;
    }

    alignedWord |= (readIn(&m_reader) << 24u);
L3:
    alignedWord |= (readIn(&m_reader) << 16u);
L2:
    alignedWord |= (readIn(&m_reader) << 8u);
L1:
    alignedWord |= (readIn(&m_reader));
L0:

    // Resolve remainder bits
    unsigned int numNextHeldBits = (32 - numBits) % 8;

    // Copy required part of alignedWord into bits
    bits |= alignedWord >> numNextHeldBits;

    // Store held bits
    m_numHeldBits = numNextHeldBits;
    /* FIXME @Jan: Can you please check, why this assert is needed?
     * I checked in ref-soft file mpegg-stream.c (in mpegg_input_bitstream_read_ubits(...)),
     * and did not see any such assert.
     */
    m_heldBits = static_cast<unsigned char>(alignedWord & 0xffu);

    return bits;
}

void BitInputStream::skipBytes(unsigned int numBytes) {
    for (unsigned int i = 0; i < numBytes; i++) {
        readIn(&m_reader);
    }
}

}  // namespace gabac

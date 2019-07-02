/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "gabac/bit_output_stream.h"

#include <cassert>

#include "gabac/data_block.h"

namespace gabac {


inline static void writeOut(
        unsigned char byte,
        DataBlock *const bitstream
){
    bitstream->push_back(byte);
}


BitOutputStream::BitOutputStream(
        DataBlock *const bitstream
)
        : m_bitstream(bitstream), m_heldBits(0), m_numHeldBits(0){
    // Nothing to do here
}


BitOutputStream::~BitOutputStream() = default;


void BitOutputStream::flush(){
    writeAlignZero();
}


void BitOutputStream::write(
        unsigned int bits,
        unsigned int numBits
){
    assert(numBits <= 32);
    // TODO(Jan): what does this assertions check?
    // assert((numBits == 32) || ((bits & (~0 << numBits)) == 0));

    // Any modulo-8 remainder of numTotalBits cannot be written this time,
    // and will be held until next time
    unsigned int numTotalBits = numBits + m_numHeldBits;
    unsigned int numNextHeldBits = numTotalBits % 8;

    // Next steps: form a byte-aligned word by concatenating any held bits
    // with the new bits, discarding the bits that will form the nextheldBits

    // Determine the nextHeldBits
    auto nextHeldBits = static_cast<unsigned char>((bits << (8u - numNextHeldBits)) & static_cast<unsigned char>(0xff));
    if (numTotalBits < 8) {
        // Insufficient bits accumulated to write out, append nextHeldBits to
        // current heldBits
        m_heldBits |= nextHeldBits;
        m_numHeldBits = numNextHeldBits;

        return;
    }

    // topword serves to justify heldBits to align with the MSB of bits
    unsigned int topword = (numBits - numNextHeldBits) & ~((1u << 3u) - 1u);
    unsigned int writeBits = (m_heldBits << topword);
    writeBits |= (bits >> numNextHeldBits);

    // Write everything
    // 1 byte / L1 is the most common case, check for it first
    if ((numTotalBits >> 3u) == 1) {
        goto L1;
    } else if ((numTotalBits >> 3u) == 2) {
        goto L2;
    } else if ((numTotalBits >> 3u) == 3) {
        goto L3;
    } else if ((numTotalBits >> 3u) != 4) {
        goto L0;
    }

    writeOut(static_cast<unsigned char> ((writeBits >> 24u) & 0xffu), m_bitstream);
    L3:
    writeOut(static_cast<unsigned char> ((writeBits >> 16u) & 0xffu), m_bitstream);
    L2:
    writeOut(static_cast<unsigned char> ((writeBits >> 8u) & 0xffu), m_bitstream);
    L1:
    writeOut(static_cast<unsigned char> (writeBits & 0xffu), m_bitstream);
    L0:

    // Update output bitstream state
    m_heldBits = nextHeldBits;
    m_numHeldBits = numNextHeldBits;
}


void BitOutputStream::writeAlignZero(){
    if (m_numHeldBits == 0) {
        return;
    }

    write(m_heldBits, (8 - m_numHeldBits));
    m_heldBits = 0x00;
    m_numHeldBits = 0;
}


}  // namespace gabac

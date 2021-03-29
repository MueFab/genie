/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/bitwriter.h"
#include <string>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

BitWriter::BitWriter(std::ostream *str) : stream(str), m_heldBits(0), m_numHeldBits(0), m_bitsWritten(0) {}

// ---------------------------------------------------------------------------------------------------------------------

BitWriter::~BitWriter() { flush(); }

// ---------------------------------------------------------------------------------------------------------------------

inline void BitWriter::writeOut(uint8_t byte) {
    stream->write(reinterpret_cast<char *>(&byte), 1);
    m_bitsWritten += 8;
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::write(uint64_t bits, uint8_t numBits) {
    // Any modulo-8 remainder of numTotalBits cannot be written this time,
    // and will be held until next time
    uint8_t numTotalBits = numBits + m_numHeldBits;
    auto numNextHeldBits = uint8_t(numTotalBits % 8);

    // Next steps: form a byte-ALIGNED word by concatenating any held bits
    // with the new bits, discarding the bits that will form the nextheldBits

    // Determine the nextHeldBits
    auto nextHeldBits = static_cast<uint8_t>((bits << (8u - numNextHeldBits)) & static_cast<uint8_t>(0xff));
    if (numTotalBits < 8) {
        // Insufficient bits accumulated to write out, append nextHeldBits to
        // current heldBits
        m_heldBits |= nextHeldBits;
        m_numHeldBits = numNextHeldBits;

        return;
    }

    // topword serves to justify heldBits to align with the MSB of bits
    uint64_t topword = uint64_t(numBits - numNextHeldBits) & uint64_t(~((1u << 3u) - 1u));
    uint64_t writeBits = (m_heldBits << topword);
    writeBits |= (bits >> numNextHeldBits);

    // Write everything
    // 1 byte / L1 is the most common case, check for it first
    if ((numTotalBits >> 3u) == 1) {
        goto L1;
    } else if ((numTotalBits >> 3u) == 2) {
        goto L2;
    } else if ((numTotalBits >> 3u) == 3) {
        goto L3;
    } else if ((numTotalBits >> 3u) == 4) {
        goto L4;
    } else if ((numTotalBits >> 3u) == 5) {
        goto L5;
    } else if ((numTotalBits >> 3u) == 6) {
        goto L6;
    } else if ((numTotalBits >> 3u) == 7) {
        goto L7;
    } else if ((numTotalBits >> 3u) != 8) {
        goto L0;
    }

    writeOut(static_cast<uint8_t>((writeBits >> 56u) & 0xffu));
L7:
    writeOut(static_cast<uint8_t>((writeBits >> 48u) & 0xffu));
L6:
    writeOut(static_cast<uint8_t>((writeBits >> 40u) & 0xffu));
L5:
    writeOut(static_cast<uint8_t>((writeBits >> 32u) & 0xffu));
L4:
    writeOut(static_cast<uint8_t>((writeBits >> 24u) & 0xffu));
L3:
    writeOut(static_cast<uint8_t>((writeBits >> 16u) & 0xffu));
L2:
    writeOut(static_cast<uint8_t>((writeBits >> 8u) & 0xffu));
L1:
    writeOut(static_cast<uint8_t>(writeBits & 0xffu));
L0:

    // Update output bitstream state
    m_heldBits = nextHeldBits;
    m_numHeldBits = numNextHeldBits;
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::write(const std::string &string) {
    for (const auto &a : string) {
        write(uint8_t(a), 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::write(std::istream *in) {
    while (true) {
        char byte = 0;
        in->read(&byte, 1);
        if (!*in) {
            break;
        }
        write(uint8_t(byte), 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::flush() {
    if (m_numHeldBits == 0) {
        return;
    }

    write(m_heldBits, uint8_t(8 - m_numHeldBits));
    m_heldBits = 0x00;
    m_numHeldBits = 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BitWriter::getBitsWritten() const { return m_bitsWritten + m_numHeldBits; }

// ---------------------------------------------------------------------------------------------------------------------

bool BitWriter::isAligned() const { return m_numHeldBits == 0; }

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::writeBypass(std::istream *in) {
    if (!isAligned()) {
        UTILS_DIE("Writer not aligned when it should be");
    }
    const size_t BUFFERSIZE = 100;
    char byte[BUFFERSIZE];
    do {
        in->read(byte, BUFFERSIZE);
        stream->write(byte, in->gcount());
        this->m_bitsWritten += in->gcount() * 8;
    } while (in->gcount() == BUFFERSIZE);
}

// ---------------------------------------------------------------------------------------------------------------------

void BitWriter::writeBypass(const void *in, size_t size) {
    this->m_bitsWritten += size * 8;
    if (!isAligned()) {
        UTILS_DIE("Writer not aligned when it should be");
    }
    stream->write(reinterpret_cast<const char *>(in), size);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

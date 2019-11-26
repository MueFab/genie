/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "binary-arithmetic-decoder.h"
#include <cassert>
#include "bit-input-stream.h"
#include "cabac-tables.h"
#include "context-model.h"

namespace gabac {

BinaryArithmeticDecoder::BinaryArithmeticDecoder(const BitInputStream& bitInputStream)
    : m_bitInputStream(bitInputStream), m_numBitsNeeded(0), m_range(0), m_value(0) {}

inline unsigned int BinaryArithmeticDecoder::decodeBin(ContextModel* const contextModel) {
    assert(contextModel != nullptr);

    unsigned int decodedByte;
    unsigned int lps = cabactables::lpsTable[contextModel->getState()][(m_range >> 6u) - 4];
    m_range -= lps;
    unsigned int scaledRange = m_range << 7u;
    if (m_value < scaledRange) {
        decodedByte = contextModel->getMps();
        contextModel->updateMps();
        if (scaledRange >= (256u << 7u)) {
            return decodedByte;
        }
        m_range = scaledRange >> 6u;
        m_value <<= 1;

        if (++m_numBitsNeeded == 0) {
            m_numBitsNeeded = -8;
            m_value += m_bitInputStream.readByte();
        }
    } else {
        unsigned int numBits = cabactables::renormTable[(lps >> 3u)];
        m_value = (m_value - scaledRange) << numBits;
        m_range = (lps << numBits);
        decodedByte = 1 - static_cast<unsigned>(contextModel->getMps());
        contextModel->updateLps();
        m_numBitsNeeded += numBits;
        if (m_numBitsNeeded >= 0) {
            m_value += m_bitInputStream.readByte() << static_cast<unsigned int>(m_numBitsNeeded);
            m_numBitsNeeded -= 8;
        }
    }

    return decodedByte;
}

unsigned int BinaryArithmeticDecoder::decodeBinsEP(unsigned int numBins) {
    unsigned int bins = 0;
    unsigned int scaledRange;
    while (numBins > 8) {
        m_value = (m_value << 8u) + (m_bitInputStream.readByte() << (8u + m_numBitsNeeded));
        scaledRange = m_range << 15u;
        for (int i = 0; i < 8; i++) {
            bins <<= 1;
            scaledRange >>= 1;
            if (m_value >= scaledRange) {
                bins++;
                m_value -= scaledRange;
            }
        }
        numBins -= 8;
    }
    m_numBitsNeeded += numBins;
    m_value <<= numBins;
    if (m_numBitsNeeded >= 0) {
        m_value += m_bitInputStream.readByte() << static_cast<unsigned int>(m_numBitsNeeded);
        m_numBitsNeeded -= 8;
    }
    scaledRange = m_range << (numBins + 7);
    for (unsigned int i = 0; i < numBins; i++) {
        bins <<= 1;
        scaledRange >>= 1;
        if (m_value >= scaledRange) {
            bins++;
            m_value -= scaledRange;
        }
    }

    return bins;
}

void BinaryArithmeticDecoder::decodeBinTrm() {
    m_range -= 2;
    unsigned int scaledRange = m_range << 7u;
    if (m_value >= scaledRange) {
        // bin = 1;
    } else {
        // bin = 0;
        if (scaledRange < (256u << 7u))  // spec: ivlCurrRange < 256
        {
            m_range = scaledRange >> 6u;  // spec: ivlCurrRange << 1
            m_value <<= 1;                // spec: ivlOffset = ivlOffset << 1
            if (++m_numBitsNeeded == 0) {
                m_numBitsNeeded = -8;
                m_value += m_bitInputStream.readByte();
            }
        }
    }
}

void BinaryArithmeticDecoder::reset() { decodeBinTrm(); }

void BinaryArithmeticDecoder::start() {
    assert(m_bitInputStream.getNumBitsUntilByteAligned() == 0);
    m_bitInputStream.skipBytes(4);  // Corresponding the numSymbols already reader::start()

    m_numBitsNeeded = -8;
    m_range = 510;
    m_value = (m_bitInputStream.readByte() << 8u);
    m_value |= m_bitInputStream.readByte();
}

}  // namespace gabac

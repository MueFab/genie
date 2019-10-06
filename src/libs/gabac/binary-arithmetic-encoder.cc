/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "binary-arithmetic-encoder.h"
#include <cassert>
#include "cabac-tables.h"

namespace gabac {

BinaryArithmeticEncoder::BinaryArithmeticEncoder(const util::BitWriter& bitOutputStream)
    : m_bitOutputStream(bitOutputStream),
      m_bufferedByte(0),
      m_low(0),
      m_numBitsLeft(0),
      m_numBufferedBytes(0),
      m_range(0) {}

BinaryArithmeticEncoder::~BinaryArithmeticEncoder() = default;

inline void BinaryArithmeticEncoder::encodeBin(unsigned int bin, ContextModel* const contextModel) {
    assert((bin == 0) || (bin == 1));
    assert(contextModel != nullptr);

    unsigned int lps = cabactables::lpsTable[contextModel->getState()][(m_range >> 6u) & 3u];
    m_range -= lps;
    if (bin != contextModel->getMps()) {
        unsigned int numBits = cabactables::renormTable[(lps >> 3u)];
        m_low = (m_low + m_range) << numBits;
        m_range = lps << numBits;
        contextModel->updateLps();
        m_numBitsLeft -= numBits;
    } else {
        contextModel->updateMps();
        if (m_range >= 256) {
            return;
        }
        m_low <<= 1;
        m_range <<= 1;
        m_numBitsLeft -= 1;
    }
    if (m_numBitsLeft < 12) {
        writeOut();
    }
}

void BinaryArithmeticEncoder::encodeBinEP(unsigned int bin) {
    assert((bin == 0) || (bin == 1));

    m_low <<= 1;
    if (bin > 0) {
        m_low += m_range;
    }
    m_numBitsLeft -= 1;
    if (m_numBitsLeft < 12) {
        writeOut();
    }
}

void BinaryArithmeticEncoder::encodeBinsEP(unsigned int bins, unsigned int numBins) {
    while (numBins > 8) {
        numBins -= 8;
        unsigned int pattern = bins >> numBins;
        m_low <<= 8;
        m_low += (m_range * pattern);
        bins -= (pattern << numBins);
        m_numBitsLeft -= 8;
        if (m_numBitsLeft < 12) {
            writeOut();
        }
    }
    m_low <<= numBins;
    m_low += (m_range * bins);
    m_numBitsLeft -= numBins;
    if (m_numBitsLeft < 12) {
        writeOut();
    }
}

void BinaryArithmeticEncoder::encodeBinTrm(unsigned int bin) {
    // Encode the least-significant bit of bin as a terminating bin
    m_range -= 2;
    if (bin != 0) {
        m_low += m_range;
        m_low <<= 7;
        m_range = (2u << 7u);
        m_numBitsLeft -= 7;
    } else if (m_range >= 256) {
        return;
    } else {
        m_low <<= 1;
        m_range <<= 1;
        m_numBitsLeft -= 1;
    }
    if (m_numBitsLeft < 12) {
        writeOut();
    }
}

void BinaryArithmeticEncoder::flush() {
    encodeBinTrm(1);
    finish();
    m_bitOutputStream.write(1, 1);
    m_bitOutputStream.flush();
    start();
}

void BinaryArithmeticEncoder::start() {
    m_bufferedByte = 0xff;
    m_low = 0;
    m_numBitsLeft = 23;
    m_numBufferedBytes = 0;
    m_range = 510;
}

void BinaryArithmeticEncoder::finish() {
    if ((m_low >> (32u - m_numBitsLeft)) > 0) {
        m_bitOutputStream.write((m_bufferedByte + 1), 8);
        while (m_numBufferedBytes > 1) {
            m_bitOutputStream.write(0x00, 8);
            m_numBufferedBytes -= 1;
        }
        m_low -= (1u << (32u - m_numBitsLeft));
    } else {
        if (m_numBufferedBytes > 0) {
            m_bitOutputStream.write(m_bufferedByte, 8);
        }
        while (m_numBufferedBytes > 1) {
            m_bitOutputStream.write(0xff, 8);
            m_numBufferedBytes -= 1;
        }
    }
    m_bitOutputStream.write(m_low >> 8u, (24u - m_numBitsLeft));
}

void BinaryArithmeticEncoder::writeOut() {
    unsigned int leadByte = m_low >> (24u - m_numBitsLeft);
    m_numBitsLeft += 8;
    m_low &= 0xffffffffu >> m_numBitsLeft;
    if (leadByte == 0xff) {
        m_numBufferedBytes += 1;
    } else {
        if (m_numBufferedBytes > 0) {
            auto carry = static_cast<unsigned char>(leadByte >> 8u);
            unsigned char byte = m_bufferedByte + carry;

            m_bufferedByte = static_cast<unsigned char>(leadByte & 0xffu);
            m_bitOutputStream.write(byte, 8);

            byte = static_cast<unsigned char>(0xff) + carry;
            while (m_numBufferedBytes > 1) {
                m_bitOutputStream.write(byte, 8);
                m_numBufferedBytes -= 1;
            }
        } else {
            m_numBufferedBytes = 1;
            m_bufferedByte = static_cast<unsigned char>(leadByte & 0xffu);
        }
    }
}

}  // namespace gabac

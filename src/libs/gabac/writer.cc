/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "writer.h"

#include <cassert>
#include <limits>

#include "constants.h"
#include "context-selector.h"

//
// #include binary-arithmetic-decoder.cc from here instead of compiling it
// separately, so that we may call inlined member functions of class
// BinaryArithmeticDecoder in this file.
//
#include "binary-arithmetic-encoder.cc"
#include "gabac.h"

namespace gabac {

static unsigned int bitLength(uint64_t value) {
    unsigned int numBits = 0;

    if (value > 0x7FFF) {
        value >>= 16;
        numBits += 16;
    }

    if (value > 0x7F) {
        value >>= 8;
        numBits += 8;
    }

    if (value > 0x7) {
        value >>= 4;
        numBits += 4;
    }

    if (value > 0x1) {
        value >>= 2;
        numBits += 2;
    }

    if (value > 0x0) {
        numBits += 1;
    }

    return numBits;
}

Writer::Writer(OBufferStream *const bitstream)
    : m_bitOutputStream(bitstream),
      m_binaryArithmeticEncoder(m_bitOutputStream),
      m_contextModels(contexttables::buildContextTable()) {}

Writer::~Writer() = default;

void Writer::start(size_t numSymbols) {
    assert(numSymbols > 0);
    assert(numSymbols <= std::numeric_limits<unsigned>::max());
    writeNumSymbols(static_cast<unsigned>(numSymbols));
    m_binaryArithmeticEncoder.start();
}

void Writer::reset() {
    m_binaryArithmeticEncoder.flush();
    m_contextModels = contexttables::buildContextTable();
}

void Writer::writeAsBIbypass(uint64_t input, unsigned int cLength) {
    assert(getBinarization(BinarizationId::BI).sbCheck(input, input, cLength));
    m_binaryArithmeticEncoder.encodeBinsEP(static_cast<unsigned int>(input), cLength);
}

void Writer::writeAsBIcabac(uint64_t input, unsigned int cLength, unsigned int offset) {
    assert(getBinarization(BinarizationId::BI).sbCheck(input, input, cLength));

    unsigned int cm = ContextSelector::getContextForBi(offset, 0);
    auto scan = m_contextModels.begin() + cm;
    for (int i = cLength - 1; i >= 0; i--)  // i must be signed
    {
        unsigned int bin = static_cast<unsigned int>(static_cast<uint64_t>(input) >> static_cast<uint8_t>(i)) & 0x1u;
        m_binaryArithmeticEncoder.encodeBin(bin, &*(scan++));
    }
}

void Writer::writeAsTUbypass(uint64_t input, unsigned int cMax) {
    assert(getBinarization(BinarizationId::TU).sbCheck(input, input, cMax));

    for (uint64_t i = 0; i < input; i++) {
        m_binaryArithmeticEncoder.encodeBinEP(1);
    }
    if (input != cMax) {
        m_binaryArithmeticEncoder.encodeBinEP(0);
    }
}

void Writer::writeAsTUcabac(uint64_t input, unsigned int cMax, unsigned int offset) {
    assert(getBinarization(BinarizationId::TU).sbCheck(input, input, cMax));

    unsigned int cm = ContextSelector::getContextForTu(offset, 0);

    auto scan = m_contextModels.begin() + cm;

    if (input == cMax) {
        for (; input > 0; input--) {
            m_binaryArithmeticEncoder.encodeBin(1, &*(scan++));
        }
    } else {
        for (; input > 0; input--) {
            m_binaryArithmeticEncoder.encodeBin(1, &*(scan++));
        }
        m_binaryArithmeticEncoder.encodeBin(0, &*scan);
    }
}

void Writer::writeAsEGbypass(uint64_t input, unsigned int) {
    assert(getBinarization(BinarizationId::EG).sbCheck(input, input, 0));

    input++;
    unsigned int length = ((bitLength(static_cast<uint64_t>(input)) - 1) << 1u) + 1;
    assert(input <= std::numeric_limits<unsigned>::max());
    m_binaryArithmeticEncoder.encodeBinsEP(static_cast<unsigned>(input), length);
}

void Writer::writeAsEGcabac(uint64_t input, unsigned int, unsigned int offset) {
    assert(getBinarization(BinarizationId::EG).sbCheck(input, input, 0));

    input++;
    unsigned int i = 0;

    unsigned int cm = ContextSelector::getContextForEg(offset, i);
    auto scan = m_contextModels.begin() + cm;
    unsigned int length = ((bitLength(static_cast<uint64_t>(input)) - 1) << 1u) + 1;

    for (i = static_cast<uint8_t>(length) >> 1u; i > 0; i--) {
        m_binaryArithmeticEncoder.encodeBin(0, &*(scan++));
    }
    m_binaryArithmeticEncoder.encodeBin(1, &*scan);

    length -= ((static_cast<uint8_t>(length) >> 1u) + 1);
    if (length != 0) {
        input -= (1u << length);
        assert(input <= std::numeric_limits<unsigned>::max());
        m_binaryArithmeticEncoder.encodeBinsEP(static_cast<unsigned>(input), length);
    }
}

void Writer::writeAsSEGbypass(uint64_t input, unsigned int) {
    assert(getBinarization(BinarizationId::SEG).sbCheck(input, input, 0));
    if (int64_t(input) <= 0) {
        writeAsEGbypass(static_cast<unsigned int>(-int64_t(input)) << 1u, 0);
    } else {
        writeAsEGbypass(static_cast<unsigned int>(static_cast<uint64_t>(input) << 1u) - 1, 0);
    }
}

void Writer::writeAsSEGcabac(uint64_t input, unsigned int, unsigned int offset) {
    assert(getBinarization(BinarizationId::SEG).sbCheck(uint64_t(input), uint64_t(input), 0));

    if (int64_t(input) <= 0) {
        writeAsEGcabac(static_cast<unsigned int>(-int64_t(input)) << 1u, 0, offset);
    } else {
        writeAsEGcabac(static_cast<unsigned int>(static_cast<uint64_t>(input) << 1u) - 1, 0, offset);
    }
}

void Writer::writeAsTEGbypass(uint64_t input, unsigned int cTruncExpGolParam) {
    assert(getBinarization(BinarizationId::TEG).sbCheck(input, input, cTruncExpGolParam));

    if (input < cTruncExpGolParam) {
        writeAsTUbypass(input, cTruncExpGolParam);
    } else {
        writeAsTUbypass(cTruncExpGolParam, cTruncExpGolParam);
        writeAsEGbypass(input - cTruncExpGolParam, 0);
    }
}

void Writer::writeAsTEGcabac(uint64_t input, unsigned int cTruncExpGolParam, unsigned int offset) {
    assert(getBinarization(BinarizationId::TEG).sbCheck(input, input, cTruncExpGolParam));

    if (input < cTruncExpGolParam) {
        writeAsTUcabac(input, cTruncExpGolParam, offset);
    } else {
        writeAsTUcabac(cTruncExpGolParam, cTruncExpGolParam, offset);
        writeAsEGcabac(input - cTruncExpGolParam, 0, offset);
    }
}

void Writer::writeAsSTEGbypass(uint64_t input, unsigned int cSignedTruncExpGolParam) {
    assert(getBinarization(BinarizationId::STEG).sbCheck(uint64_t(input), uint64_t(input), cSignedTruncExpGolParam));

    if (int64_t(input) < 0) {
        writeAsTEGbypass(uint64_t(-int64_t(input)), cSignedTruncExpGolParam);
        writeAsBIbypass(1, 1);
    } else if (input > 0) {
        writeAsTEGbypass(uint64_t(input), cSignedTruncExpGolParam);
        writeAsBIbypass(0, 1);
    } else {
        writeAsTEGbypass(0, cSignedTruncExpGolParam);
    }
}

void Writer::writeAsSTEGcabac(uint64_t input, unsigned int cSignedTruncExpGolParam, unsigned int offset) {
    assert(getBinarization(BinarizationId::STEG).sbCheck(uint64_t(input), uint64_t(input), cSignedTruncExpGolParam));

    if (int64_t(input) < 0) {
        writeAsTEGcabac(uint64_t(-int64_t(input)), cSignedTruncExpGolParam, offset);
        writeAsBIcabac(1, 1, offset);
    } else if (input > 0) {
        writeAsTEGcabac(uint64_t(input), cSignedTruncExpGolParam, offset);
        writeAsBIcabac(0, 1, offset);
    } else {
        writeAsTEGcabac(0, cSignedTruncExpGolParam, offset);
    }
}

void Writer::writeNumSymbols(unsigned int numSymbols) { m_bitOutputStream.write(numSymbols, 32); }

}  // namespace gabac

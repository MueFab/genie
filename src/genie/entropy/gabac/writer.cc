/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "writer.h"

#include <cassert>
#include <limits>

#include "context-tables.h"
//
// #include binary-arithmetic-decoder.cc from here instead of compiling it
// separately, so that we may call inlined member functions of class
// BinaryArithmeticDecoder in this file.
//
#include "binary-arithmetic-encoder.cc"

namespace genie {
namespace entropy {
namespace gabac {

static unsigned int getBitLength(uint64_t value) {
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

Writer::Writer(OBufferStream *const bitstream, const bool bypassFlag, const unsigned long numContexts)
    : m_bitOutputStream(bitstream),
      m_binaryArithmeticEncoder(m_bitOutputStream),
      m_bypassFlag(bypassFlag),
      m_numContexts(numContexts) {
    if(!bypassFlag && numContexts > 0) {
        m_contextModels = contexttables::buildContextTable(m_numContexts);
    }
}

Writer::~Writer() = default;

void Writer::start(const size_t numSymbols) {
    assert(numSymbols > 0);
    assert(numSymbols <= std::numeric_limits<unsigned>::max());
    writeNumSymbols(static_cast<unsigned>(numSymbols));
    m_binaryArithmeticEncoder.start();
}

void Writer::close() {
    m_binaryArithmeticEncoder.flush();
}

void Writer::reset() {
    m_binaryArithmeticEncoder.flush();
    m_contextModels.clear();
    m_contextModels = contexttables::buildContextTable(m_numContexts);
}

void Writer::writeAsBIbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::BI).sbCheck(input, input, cLength));
    m_binaryArithmeticEncoder.encodeBinsEP(static_cast<unsigned int>(input), binParams[0]);
}

void Writer::writeAsBIcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::BI).sbCheck(input, input, cLength));

    const unsigned int cLength = binParams[0];
    unsigned int cm = ctxIdx;
    auto scan = m_contextModels.begin() + cm;
    for (int i = cLength - 1; i >= 0; i--)  // i must be signed
    {
        unsigned int bin = static_cast<unsigned int>(static_cast<uint64_t>(input) >> static_cast<uint8_t>(i)) & 0x1u;
        m_binaryArithmeticEncoder.encodeBin(bin, &*(scan++));
    }
}

void Writer::writeAsTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::TU).sbCheck(input, input, cMax));

    const unsigned int cMax = binParams[0];
    for (uint64_t i = 0; i < input; i++) {
        m_binaryArithmeticEncoder.encodeBinEP(1);
    }
    if (input != cMax) {
        m_binaryArithmeticEncoder.encodeBinEP(0);
    }
}

void Writer::writeAsTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::TU).sbCheck(input, input, cMax));

    const unsigned int cMax = binParams[0];
    unsigned int cm = ctxIdx;
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

void Writer::writeAsEGbypass(uint64_t input, const std::vector<unsigned int>) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::EG).sbCheck(input, input, 0));

    input++;
    unsigned int length = ((getBitLength(static_cast<uint64_t>(input)) - 1) << 1u) + 1;
    assert(input <= std::numeric_limits<unsigned>::max());
    m_binaryArithmeticEncoder.encodeBinsEP(static_cast<unsigned>(input), length);
}

void Writer::writeAsEGcabac(uint64_t input, const std::vector<unsigned int>, const unsigned int ctxIdx) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::EG).sbCheck(input, input, 0));

    input++;
    unsigned int i = 0;

    unsigned int cm = ctxIdx;
    auto scan = m_contextModels.begin() + cm;
    unsigned int length = ((getBitLength(static_cast<uint64_t>(input)) - 1) << 1u) + 1;

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

void Writer::writeAsSEGbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::SEG).sbCheck(input, input, 0));
    if (int64_t(input) <= 0) {
        writeAsEGbypass(static_cast<unsigned int>(-int64_t(input)) << 1u, binParams);
    } else {
        writeAsEGbypass(static_cast<unsigned int>(static_cast<uint64_t>(input) << 1u) - 1, binParams);
    }
}

void Writer::writeAsSEGcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::SEG).sbCheck(uint64_t(input), uint64_t(input), 0));

    if (int64_t(input) <= 0) {
        writeAsEGcabac(static_cast<unsigned int>(-int64_t(input)) << 1u, binParams, ctxIdx);
    } else {
        writeAsEGcabac(static_cast<unsigned int>(static_cast<uint64_t>(input) << 1u) - 1, binParams, ctxIdx);
    }
}

void Writer::writeAsTEGbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::TEG).sbCheck(input, input, cTruncExpGolParam));

    const unsigned int cTruncExpGolParam = binParams[0];
    if (input < cTruncExpGolParam) {
        writeAsTUbypass(input, binParams);
    } else {
        writeAsTUbypass(cTruncExpGolParam, binParams);
        writeAsEGbypass(input - cTruncExpGolParam, std::vector<unsigned int>({0}));
    }
}

void Writer::writeAsTEGcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::TEG).sbCheck(input, input, cTruncExpGolParam));

    const unsigned int cTruncExpGolParam = binParams[0];
    if (input < cTruncExpGolParam) {
        writeAsTUcabac(input, binParams, ctxIdx);
    } else {
        writeAsTUcabac(cTruncExpGolParam, binParams, ctxIdx);
        writeAsEGcabac(input - cTruncExpGolParam, std::vector<unsigned int>({0}), ctxIdx);
    }
}

void Writer::writeAsSTEGbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::STEG).sbCheck(uint64_t(input), uint64_t(input), cSignedTruncExpGolParam));

    if (int64_t(input) < 0) {
        writeAsTEGbypass(uint64_t(-int64_t(input)), binParams);
        writeAsBIbypass(1, std::vector<unsigned int>({1}));
    } else if (input > 0) {
        writeAsTEGbypass(uint64_t(input), binParams);
        writeAsBIbypass(0, std::vector<unsigned int>({1}));
    } else {
        writeAsTEGbypass(0, binParams);
    }
}

void Writer::writeAsSTEGcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    //RESTRUCT_DISABLE assert(getBinarization(BinarizationId::STEG).sbCheck(uint64_t(input), uint64_t(input), cSignedTruncExpGolParam));

    if (int64_t(input) < 0) {
        writeAsTEGcabac(uint64_t(-int64_t(input)), binParams, ctxIdx);
        writeAsBIcabac(1, std::vector<unsigned int>({1}), ctxIdx); // FIXME fix ctxIdx for sign bit
    } else if (input > 0) {
        writeAsTEGcabac(uint64_t(input), binParams, ctxIdx);
        writeAsBIcabac(0, std::vector<unsigned int>({1}), ctxIdx); // FIXME fix ctxIdx for sign bit
    } else {
        writeAsTEGcabac(0, binParams, ctxIdx);
    }
}

void Writer::writeAsSUTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {

    const unsigned int outputSymSize = binParams[0];
    const unsigned int splitUnitSize = binParams[1];

    unsigned int i, j;
    for(i=0, j=outputSymSize; i<outputSymSize; i+=splitUnitSize) {
        unsigned int unitSize = (i == 0 && outputSymSize % splitUnitSize)
                                ? outputSymSize % splitUnitSize
                                : splitUnitSize;
        unsigned int cMax = (1u<<unitSize)-1;
        unsigned int val = (input>>(j-=unitSize)) & cMax;
        writeAsTUbypass(val, std::vector<unsigned int>({cMax}));
    }
}

void Writer::writeAsSUTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {

    const unsigned int outputSymSize = binParams[0];
    const unsigned int splitUnitSize = binParams[1];

    unsigned int cm = ctxIdx;
    unsigned int i, j;
    for(i=0, j=outputSymSize; i<outputSymSize; i+=splitUnitSize) {
        unsigned int unitSize = (i == 0 && outputSymSize % splitUnitSize)
                                ? outputSymSize % splitUnitSize
                                : splitUnitSize;
        unsigned int cMax = (1u<<unitSize)-1;
        unsigned int val = (input>>(j-=unitSize)) & cMax;
        writeAsTUcabac(val, std::vector<unsigned int>({cMax}), cm);
        cm += cMax;
    }
}

void Writer::writeAsSSUTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {

    if (int64_t(input) < 0) {
        writeAsSUTUbypass(uint64_t(-int64_t(input)), binParams);
        writeAsBIbypass(1, std::vector<unsigned int>({1}));
    } else if (input > 0) {
        writeAsSUTUbypass(uint64_t(input), binParams);
        writeAsBIbypass(0, std::vector<unsigned int>({1}));
    } else {
        writeAsSUTUbypass(0, binParams);
    }
}

void Writer::writeAsSSUTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    if (int64_t(input) < 0) {
        writeAsSUTUcabac(uint64_t(-int64_t(input)), binParams, ctxIdx);
        writeAsBIcabac(1, std::vector<unsigned int>({1}), ctxIdx);  // FIXME fix ctxIdx for sign bit
    } else if (input > 0) {
        writeAsSUTUcabac(uint64_t(input), binParams, ctxIdx);
        writeAsBIcabac(0, std::vector<unsigned int>({1}), ctxIdx);  // FIXME fix ctxIdx for sign bit
    } else {
        writeAsSUTUcabac(0, binParams, ctxIdx);
    }
}

void Writer::writeAsDTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {

    const unsigned int cMaxDtu = binParams[2];

    writeAsTUbypass((input<cMaxDtu) ? input : cMaxDtu, std::vector<unsigned int>({cMaxDtu}));

    if(input >= cMaxDtu) {
        input -= cMaxDtu;

        writeAsSUTUbypass(input, binParams);
    }
}

void Writer::writeAsDTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {

    const unsigned int cMaxDtu = binParams[2];

    writeAsTUcabac((input<cMaxDtu) ? input : cMaxDtu, std::vector<unsigned int>({cMaxDtu}), ctxIdx);

    if(input >= cMaxDtu) {
        input -= cMaxDtu;

        writeAsSUTUcabac(input, binParams, ctxIdx + cMaxDtu);
    }
}

void Writer::writeAsSDTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {

    if (int64_t(input) < 0) {
        writeAsDTUbypass(uint64_t(-int64_t(input)), binParams);
        writeAsBIbypass(1, std::vector<unsigned int>({1}));
    } else if (input > 0) {
        writeAsDTUbypass(uint64_t(input), binParams);
        writeAsBIbypass(0, std::vector<unsigned int>({1}));
    } else {
        writeAsDTUbypass(0, binParams);
    }
}

void Writer::writeAsSDTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    if (int64_t(input) < 0) {
        writeAsDTUcabac(uint64_t(-int64_t(input)), binParams, ctxIdx);
        writeAsBIcabac(1, std::vector<unsigned int>({1}), ctxIdx);  // FIXME fix ctxIdx for sign bit
    } else if (input > 0) {
        writeAsDTUcabac(uint64_t(input), binParams, ctxIdx);
        writeAsBIcabac(0, std::vector<unsigned int>({1}), ctxIdx);  // FIXME fix ctxIdx for sign bit
    } else {
        writeAsDTUcabac(0, binParams, ctxIdx);
    }
}

void Writer::writeNumSymbols(unsigned int numSymbols) { m_bitOutputStream.write(numSymbols, 32); }
}  // namespace gabac
}  // namespace entropy
}  // namespace genie

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "writer.h"

#include <cassert>
#include <cmath>
#include <limits>

#include "context-tables.h"
//
// #include binary-arithmetic-decoder.cc from here instead of compiling it
// separately, so that we may call inlined member functions of class
// BinaryArithmeticDecoder in this file.
//
#include "binary-arithmetic-encoder.cc"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

Writer::Writer(OBufferStream *const bitstream, const bool bypassFlag, const unsigned long numContexts)
    : m_bitOutputStream(bitstream),
      m_binaryArithmeticEncoder(m_bitOutputStream),
      m_bypassFlag(bypassFlag),
      m_numContexts(numContexts) {
    if (!bypassFlag && numContexts > 0) {
        m_contextModels = contexttables::buildContextTable(m_numContexts);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Writer::~Writer() = default;

// ---------------------------------------------------------------------------------------------------------------------

void Writer::start() { m_binaryArithmeticEncoder.start(); }

// ---------------------------------------------------------------------------------------------------------------------

void Writer::close() { m_binaryArithmeticEncoder.flush(); }

// ---------------------------------------------------------------------------------------------------------------------

void Writer::reset() {
    m_binaryArithmeticEncoder.flush();
    m_contextModels.clear();
    m_contextModels = contexttables::buildContextTable(m_numContexts);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsBIbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    m_binaryArithmeticEncoder.encodeBinsEP(static_cast<unsigned int>(input), binParams[0]);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsBIcabac(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int cLength = binParams[0];
    unsigned int cm = binParams[3];
    auto scan = m_contextModels.begin() + cm;
    for (int i = cLength - 1; i >= 0; i--)  // i must be signed
    {
        unsigned int bin = static_cast<unsigned int>(static_cast<uint64_t>(input) >> static_cast<uint8_t>(i)) & 0x1u;
        m_binaryArithmeticEncoder.encodeBin(bin, &*(scan++));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int cMax = binParams[0];
    for (uint64_t i = 0; i < input; i++) {
        m_binaryArithmeticEncoder.encodeBinEP(1);
    }
    if (cMax > input) {
        m_binaryArithmeticEncoder.encodeBinEP(0);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsTUcabac(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int cMax = binParams[0];
    unsigned int cm = binParams[3];
    auto scan = m_contextModels.begin() + cm;
    for (uint64_t i = 0; i < input; i++) {
        m_binaryArithmeticEncoder.encodeBin(1, &*(scan++));
    }
    if (cMax > input) {
        m_binaryArithmeticEncoder.encodeBin(0, &*scan);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsEGbypass(uint64_t input, const std::vector<unsigned int>) {
    unsigned int valuePlus1 = input + 1;
    unsigned int numLeadZeros = floor(log2(valuePlus1));

    /* prefix */
    writeAsBIbypass(1, std::vector<unsigned int>({numLeadZeros + 1}));
    if (numLeadZeros) {
        /* suffix */
        writeAsBIbypass(valuePlus1 & ((1u << numLeadZeros) - 1), std::vector<unsigned int>({numLeadZeros}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsEGcabac(uint64_t input, const std::vector<unsigned int> binParams) {
    unsigned int valuePlus1 = input + 1;
    unsigned int numLeadZeros = floor(log2(valuePlus1));

    /* prefix */
    writeAsBIcabac(1, std::vector<unsigned int>({numLeadZeros + 1, 0, 0, binParams[3]}));
    if (numLeadZeros) {
        /* suffix */
        writeAsBIbypass(valuePlus1 & ((1u << numLeadZeros) - 1), std::vector<unsigned int>({numLeadZeros}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsTEGbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int cTruncExpGolParam = binParams[0];
    if (input < cTruncExpGolParam) {
        writeAsTUbypass(input, binParams);
    } else {
        writeAsTUbypass(cTruncExpGolParam, binParams);
        writeAsEGbypass(input - cTruncExpGolParam, std::vector<unsigned int>({0}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsTEGcabac(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int cTruncExpGolParam = binParams[0];

    writeAsTUcabac(input, std::vector<unsigned int>({cTruncExpGolParam, 0, 0, binParams[3]}));
    if (input >= cTruncExpGolParam) {
        writeAsEGcabac(input - cTruncExpGolParam,
                       std::vector<unsigned int>({0, 0, 0, binParams[3] + cTruncExpGolParam}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsSUTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int outputSymSize = binParams[0];
    const unsigned int splitUnitSize = binParams[1];

    unsigned int i, j;
    for (i = 0, j = outputSymSize; i < outputSymSize; i += splitUnitSize) {
        unsigned int unitSize =
            (i == 0 && outputSymSize % splitUnitSize) ? outputSymSize % splitUnitSize : splitUnitSize;
        unsigned int cMax = (1u << unitSize) - 1;
        unsigned int val = (input >> (j -= unitSize)) & cMax;
        writeAsTUbypass(val, std::vector<unsigned int>({cMax}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsSUTUcabac(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int outputSymSize = binParams[0];
    const unsigned int splitUnitSize = binParams[1];

    unsigned int cm = binParams[3];
    unsigned int i, j;
    for (i = 0, j = outputSymSize; i < outputSymSize; i += splitUnitSize) {
        unsigned int unitSize =
            (i == 0 && outputSymSize % splitUnitSize) ? outputSymSize % splitUnitSize : splitUnitSize;
        unsigned int cMax = (1u << unitSize) - 1;
        unsigned int val = (input >> (j -= unitSize)) & cMax;
        writeAsTUcabac(val, std::vector<unsigned int>({cMax, 0, 0, cm}));
        cm += cMax;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsDTUbypass(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int cMaxDtu = binParams[2];

    writeAsTUbypass((input < cMaxDtu) ? input : cMaxDtu, std::vector<unsigned int>({cMaxDtu}));

    if (input >= cMaxDtu) {
        input -= cMaxDtu;

        writeAsSUTUbypass(input, binParams);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeAsDTUcabac(uint64_t input, const std::vector<unsigned int> binParams) {
    const unsigned int cMaxDtu = binParams[2];

    writeAsTUcabac((input < cMaxDtu) ? input : cMaxDtu, std::vector<unsigned int>({cMaxDtu, 0, 0, binParams[3]}));

    if (input >= cMaxDtu) {
        input -= cMaxDtu;

        writeAsSUTUcabac(input, std::vector<unsigned int>({binParams[0], binParams[1], 0, binParams[3] + cMaxDtu}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeBI(uint64_t input, const std::vector<unsigned int> binParams) {
    if (m_bypassFlag)
        writeAsBIbypass(input, binParams);
    else
        writeAsBIcabac(input, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeTU(uint64_t input, const std::vector<unsigned int> binParams) {
    if (m_bypassFlag)
        writeAsTUbypass(input, binParams);
    else
        writeAsTUcabac(input, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeEG(uint64_t input, const std::vector<unsigned int> binParams) {
    if (m_bypassFlag)
        writeAsEGbypass(input, binParams);
    else
        writeAsEGcabac(input, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeTEG(uint64_t input, const std::vector<unsigned int> binParams) {
    if (m_bypassFlag)
        writeAsTEGbypass(input, binParams);
    else
        writeAsTEGcabac(input, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeSUTU(uint64_t input, const std::vector<unsigned int> binParams) {
    if (m_bypassFlag)
        writeAsSUTUbypass(input, binParams);
    else
        writeAsSUTUcabac(input, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeDTU(uint64_t input, const std::vector<unsigned int> binParams) {
    if (m_bypassFlag)
        writeAsDTUbypass(input, binParams);
    else
        writeAsDTUcabac(input, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeLutSymbol(uint64_t input, const uint8_t codingSubsymSize) {
    std::vector<unsigned int> binParams({codingSubsymSize, 2, 0, 0});  // ctxIdx = 0
    if (m_bypassFlag)
        writeAsSUTUbypass(input, binParams);
    else
        writeAsSUTUcabac(input, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

void Writer::writeSignFlag(int64_t input) {
    std::vector<unsigned int> binParams({1});
    if (m_bypassFlag)
        writeAsBIbypass(input < 0, std::vector<unsigned int>({1}));
    else
        writeAsBIcabac(input < 0, std::vector<unsigned int>({1, 0, 0, static_cast<unsigned int>(m_numContexts - 1)}));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
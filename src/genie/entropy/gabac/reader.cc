/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reader.h"

#include "constants.h"
#include "context-tables.h"

//
// #include binary-arithmetic-decoder.cc from here instead of compiling it
// separately, so that we may call inlined member functions of class
// BinaryArithmeticDecoder in this file.
//
#include "binary-arithmetic-decoder.cc"

namespace genie {
namespace entropy {
namespace gabac {

Reader::Reader(util::DataBlock *const bitstream, const bool bypassFlag, const unsigned long numContexts)
    : m_bitInputStream(bitstream),
      m_decBinCabac(m_bitInputStream),
      m_bypassFlag(bypassFlag),
      m_numContexts(numContexts) {
    if(!bypassFlag && numContexts > 0) {
        m_contextModels = contexttables::buildContextTable(m_numContexts);
    }
}

Reader::~Reader() = default;

uint64_t Reader::readAsBIbypass(const std::vector<unsigned int> binParams) { return m_decBinCabac.decodeBinsEP(binParams[0]); }

uint64_t Reader::readAsBIcabac(const std::vector<unsigned int> binParams) {
    unsigned int bins = 0;
    unsigned int cm = binParams[3];
    const unsigned int cLength = binParams[0];
    auto scan = m_contextModels.begin() + cm;
    for (size_t i = cLength; i > 0; i--) {
        bins = (bins << 1u) | m_decBinCabac.decodeBin(&*(scan++));
    }
    return static_cast<uint64_t>(bins);
}

uint64_t Reader::readAsTUbypass(const std::vector<unsigned int> binParams) {
    unsigned int i = 0;
    const unsigned int cMax = binParams[0];
    uint8_t b = 0;
    while (i < cMax) {
        b = m_decBinCabac.decodeBinsEP(1);
        if(b == 0)
            break;
        i++;
    }
    return static_cast<uint64_t>(i);
}

uint64_t Reader::readAsTUcabac(const std::vector<unsigned int> binParams) {
    unsigned int i = 0;
    unsigned int cm = binParams[3];
    const unsigned int cMax = binParams[0];
    auto scan = m_contextModels.begin() + cm;
    uint8_t b = 0;
    while (i < cMax) {
        b = m_decBinCabac.decodeBin(&*scan);
        if(b == 0)
            break;
        i++;
        scan++;
    }
    return static_cast<uint64_t>(i);
}

uint64_t Reader::readAsEGbypass(const std::vector<unsigned int>) {
    unsigned int bins = 0;
    unsigned int i = 0;
    while (readAsBIbypass(std::vector<unsigned int>({1})) == 0) {
        i++;
    }
    if (i != 0) {
        bins = (1u << i) | m_decBinCabac.decodeBinsEP(i);
    } else {
        return 0;
    }
    return static_cast<uint64_t>(bins - 1);
}

uint64_t Reader::readAsEGcabac(const std::vector<unsigned int> binParams) {
    unsigned int cm = binParams[3];
    auto scan = m_contextModels.begin() + cm;
    unsigned int i = 0;
    while (m_decBinCabac.decodeBin(&*scan) == 0) {
        scan++;
        i++;
    }
    unsigned int bins = 0;
    if (i != 0) {
        bins = (1u << i) | m_decBinCabac.decodeBinsEP(i);
    } else {
        return 0;
    }
    return static_cast<uint64_t>(bins - 1);
}

uint64_t Reader::readAsTEGbypass(const std::vector<unsigned int> binParams) {
    uint64_t value = readAsTUbypass(binParams);
    if (static_cast<unsigned int>(value) == binParams[0]) {
        value += readAsEGbypass(std::vector<unsigned int>({0}));
    }
    return value;
}

uint64_t Reader::readAsTEGcabac(const std::vector<unsigned int> binParams) {
    uint64_t value = readAsTUcabac(binParams);
    if (static_cast<unsigned int>(value) == binParams[0]) {
        value += readAsEGcabac(std::vector<unsigned int>({0, 0, 0, binParams[3]+binParams[0]}));
    }
    return value;
}

uint64_t Reader::readAsSUTUbypass(const std::vector<unsigned int> binParams) {
    const uint32_t outputSymSize = binParams[0];
    const uint32_t splitUnitSize = binParams[1];

    uint32_t i;
    uint64_t value = 0;

    for(i=0; i<outputSymSize; i+=splitUnitSize) {
        uint64_t val = 0;
        uint32_t cMax = (i == 0 && outputSymSize % splitUnitSize)
                      ? (1u<<(outputSymSize % splitUnitSize))-1
                      : (1u<<splitUnitSize)-1;
        val = readAsTUbypass(std::vector<unsigned int>({cMax}));

        value = (value<<splitUnitSize) | val;
    }

    return value;
}

uint64_t Reader::readAsSUTUcabac(const std::vector<unsigned int> binParams) {
    const uint32_t outputSymSize = binParams[0];
    const uint32_t splitUnitSize = binParams[1];

    uint32_t cm = binParams[3];
    uint32_t i;
    uint64_t value = 0;

    for(i=0; i<outputSymSize; i+=splitUnitSize) {
        uint64_t val = 0;
        uint32_t cMax = (i == 0 && outputSymSize % splitUnitSize)
                      ? (1u<<(outputSymSize % splitUnitSize))-1
                      : (1u<<splitUnitSize)-1;
        val = readAsTUcabac(std::vector<unsigned int>({cMax, 0, 0, cm}));
        cm += cMax;

        value = (value<<splitUnitSize) | val;
    }

    return value;
}

uint64_t Reader::readAsDTUbypass(const std::vector<unsigned int> binParams) {
    const uint32_t cMaxDTU = binParams[2];

    uint64_t value = readAsTUbypass(std::vector<unsigned int>({cMaxDTU}));

    if(value >= cMaxDTU) {
        value += readAsSUTUbypass(binParams);
    }

    return value;
}

uint64_t Reader::readAsDTUcabac(const std::vector<unsigned int> binParams) {
    const uint32_t cMaxDTU = binParams[2];

    uint64_t value = readAsTUcabac(std::vector<unsigned int>({cMaxDTU, 0, 0, binParams[3]}));

    if(value >= cMaxDTU) {
        value += readAsSUTUcabac(std::vector<unsigned int>({binParams[0], binParams[1], 0, binParams[3]+cMaxDTU}));
    }

    return value;
}

uint64_t Reader::readBI(const std::vector<unsigned int> binParams) {
    if(m_bypassFlag)
        return readAsBIbypass(binParams);
    else
        return readAsBIcabac(binParams);
}

uint64_t Reader::readTU(const std::vector<unsigned int> binParams) {
    if(m_bypassFlag)
        return readAsTUbypass(binParams);
    else
        return readAsTUcabac(binParams);
}

uint64_t Reader::readEG(const std::vector<unsigned int> binParams) {
    if(m_bypassFlag)
        return readAsEGbypass(binParams);
    else
        return readAsEGcabac(binParams);
}

uint64_t Reader::readTEG(const std::vector<unsigned int> binParams) {
    if(m_bypassFlag)
        return readAsTEGbypass(binParams);
    else
        return readAsTEGcabac(binParams);
}

uint64_t Reader::readSUTU(const std::vector<unsigned int> binParams) {
    if(m_bypassFlag)
        return readAsSUTUbypass(binParams);
    else
        return readAsSUTUcabac(binParams);
}

uint64_t Reader::readDTU(const std::vector<unsigned int> binParams) {
    if(m_bypassFlag)
        return readAsDTUbypass(binParams);
    else
        return readAsDTUcabac(binParams);
}

uint64_t Reader::readLutSymbol(const uint8_t codingSubsymSize) {
    std::vector<unsigned int> binParams({codingSubsymSize, 2, 0, 0}); // ctxIdx = 0
    if(m_bypassFlag)
        return readAsSUTUbypass(binParams);
    else
        return readAsSUTUcabac(binParams);
}

bool Reader::readSignFlag() {
    std::vector<unsigned int> binParams({1});
    if(m_bypassFlag)
        return (bool) readAsBIbypass(std::vector<unsigned int>({1}));
    else
        return (bool) readAsBIcabac(std::vector<unsigned int>({1, 0, 0, static_cast<unsigned int>(m_numContexts-1)}));
}

size_t Reader::readNumSymbols() {
    auto result = m_bitInputStream.read(32);
    return static_cast<size_t>(result);
}

size_t Reader::start() {
    size_t numSymbols = readNumSymbols();
    if (numSymbols > 0) m_decBinCabac.start();
    return numSymbols;
}

void Reader::close() {
    m_decBinCabac.reset();
}

void Reader::reset() {
    m_decBinCabac.reset();
    m_contextModels.clear();
    m_contextModels = contexttables::buildContextTable(m_numContexts);
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

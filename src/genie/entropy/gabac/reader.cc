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

#include "context-selector.h"

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

uint64_t Reader::readAsBIcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    unsigned int bins = 0;
    unsigned int cm = ctxIdx;
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
    while (readAsBIbypass(std::vector<unsigned int>({1})) == 1) {
        i++;
        if (i == cMax) {
            break;
        }
    }
    return static_cast<uint64_t>(i);
}

uint64_t Reader::readAsTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    unsigned int i = 0;
    unsigned int cm = ctxIdx;
    const unsigned int cMax = binParams[0];
    auto scan = m_contextModels.begin() + cm;
    while (m_decBinCabac.decodeBin(&*scan) == 1) {
        i++;
        if (cMax == i) {
            break;
        } else {
            scan++;
        }
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

uint64_t Reader::readAsEGcabac(const std::vector<unsigned int>, const unsigned int ctxIdx) {
    unsigned int cm = ctxIdx;
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

uint64_t Reader::readAsSEGbypass(const std::vector<unsigned int> binParams) {
    uint64_t tmp = readAsEGbypass(binParams);
    // Save, only last bit
    if ((static_cast<uint64_t>(tmp) & 0x1u) == 0) {
        if (tmp == 0) {
            return 0;
        } else {
            return static_cast<uint64_t>(-1 * static_cast<int64_t>(tmp >> 1u));
        }
    } else {
        return (static_cast<uint64_t>(tmp + 1) >> 1u);
    }
}

uint64_t Reader::readAsSEGcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    uint64_t tmp = readAsEGcabac(binParams, ctxIdx);
    if ((static_cast<uint64_t>(tmp) & 0x1u) == 0) {
        if (tmp == 0) {
            return 0;
        } else {
            return (static_cast<uint64_t>(-1 * static_cast<int64_t>(tmp >> 1u)));
        }
    } else {
        return (tmp + 1) >> 1u;
    }
}

uint64_t Reader::readAsTEGbypass(const std::vector<unsigned int> binParams) {
    uint64_t value = readAsTUbypass(binParams);
    if (static_cast<unsigned int>(value) == binParams[0]) {
        value += readAsEGbypass(std::vector<unsigned int>({0}));
    }
    return value;
}

uint64_t Reader::readAsTEGcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    uint64_t value = readAsTUcabac(binParams, ctxIdx);
    if (static_cast<unsigned int>(value) == binParams[0]) {
        value += readAsEGcabac(std::vector<unsigned int>({0}), ctxIdx); //FIXME should not this be ctxIdx+binParams[0]?
    }
    return value;
}

uint64_t Reader::readAsSTEGbypass(const std::vector<unsigned int> binParams) {
    uint64_t value = readAsTEGbypass(binParams);
    if (value != 0) {
        if (readAsBIbypass(std::vector<unsigned int>({1})) == 1) {
            return uint64_t(-1 * int64_t(value));
        } else {
            return value;
        }
    }
    return value;
}

uint64_t Reader::readAsSTEGcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    uint64_t value = readAsTEGcabac(binParams, ctxIdx);
    if (value != 0) {
        if (readAsBIcabac(std::vector<unsigned int>({1}), ctxIdx) == 1) {  // FIXME fix ctxIdx for sign bit
            return uint64_t(-1 * int64_t(value));
        } else {
            return value;
        }
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

uint64_t Reader::readAsSUTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    const uint32_t outputSymSize = binParams[0];
    const uint32_t splitUnitSize = binParams[1];

    uint32_t cm = ctxIdx;
    uint32_t i;
    uint64_t value = 0;

    for(i=0; i<outputSymSize; i+=splitUnitSize) {
        uint64_t val = 0;
        uint32_t cMax = (i == 0 && outputSymSize % splitUnitSize)
                      ? (1u<<(outputSymSize % splitUnitSize))-1
                      : (1u<<splitUnitSize)-1;
        val = readAsTUcabac(std::vector<unsigned int>({cMax}), cm);
        cm += cMax;

        value = (value<<splitUnitSize) | val;
    }

    return value;
}

uint64_t Reader::readAsSSUTUbypass(const std::vector<unsigned int> binParams) {
    uint64_t value = readAsSUTUbypass(binParams);
    if (value != 0) {
        if (readAsBIbypass(std::vector<unsigned int>({1})) == 1) {
            return uint64_t(-1 * int64_t(value));
        } else {
            return value;
        }
    }
    return value;
}

uint64_t Reader::readAsSSUTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    uint64_t value = readAsSUTUcabac(binParams, ctxIdx);
    if (value != 0) {
        if (readAsBIcabac(std::vector<unsigned int>({1}), ctxIdx) == 1) {  // FIXME fix ctxIdx for sign bit
            return uint64_t(-1 * int64_t(value));
        } else {
            return value;
        }
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

uint64_t Reader::readAsDTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    const uint32_t cMaxDTU = binParams[2];

    uint64_t value = readAsTUcabac(std::vector<unsigned int>({cMaxDTU}), ctxIdx);

    if(value >= cMaxDTU) {
        value += readAsSUTUcabac(binParams, ctxIdx + cMaxDTU);
    }

    return value;
}

uint64_t Reader::readAsSDTUbypass(const std::vector<unsigned int> binParams) {
    uint64_t value = readAsDTUbypass(binParams);
    if (value != 0) {
        if (readAsBIbypass(std::vector<unsigned int>({1})) == 1) {
            return uint64_t(-1 * int64_t(value));
        } else {
            return value;
        }
    }
    return value;
}

uint64_t Reader::readAsSDTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx) {
    uint64_t value = readAsDTUcabac(binParams, ctxIdx);
    if (value != 0) {
        if (readAsBIcabac(std::vector<unsigned int>({1}), ctxIdx) == 1) {  // FIXME fix ctxIdx for sign bit
            return uint64_t(-1 * int64_t(value));
        } else {
            return value;
        }
    }
    return value;
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

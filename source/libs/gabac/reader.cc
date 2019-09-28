/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
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

namespace gabac {

Reader::Reader(DataBlock *const bitstream)
    : m_bitInputStream(bitstream),
      // m_contextSelector(),
      m_decBinCabac(m_bitInputStream),
      m_contextModels(contexttables::buildContextTable()) {}

Reader::~Reader() = default;

uint64_t Reader::readAsBIbypass(unsigned int cLength) { return m_decBinCabac.decodeBinsEP(cLength); }

uint64_t Reader::readAsBIcabac(unsigned int cLength, unsigned int offset) {
    unsigned int bins = 0;
    unsigned int cm = ContextSelector::getContextForBi(offset, 0);
    auto scan = m_contextModels.begin() + cm;
    for (size_t i = cLength; i > 0; i--) {
        bins = (bins << 1u) | m_decBinCabac.decodeBin(&*(scan++));
    }
    return static_cast<uint64_t>(bins);
}

uint64_t Reader::readAsTUbypass(unsigned int cMax) {
    unsigned int i = 0;
    while (readAsBIbypass(1) == 1) {
        i++;
        if (i == cMax) {
            break;
        }
    }
    return static_cast<uint64_t>(i);
}

uint64_t Reader::readAsTUcabac(unsigned int cMax, unsigned int offset) {
    unsigned int i = 0;
    unsigned int cm = ContextSelector::getContextForTu(offset, i);
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

uint64_t Reader::readAsEGbypass(unsigned int) {
    unsigned int bins = 0;
    unsigned int i = 0;
    while (readAsBIbypass(1) == 0) {
        i++;
    }
    if (i != 0) {
        bins = (1u << i) | m_decBinCabac.decodeBinsEP(i);
    } else {
        return 0;
    }
    return static_cast<uint64_t>(bins - 1);
}

uint64_t Reader::readAsEGcabac(unsigned int, unsigned int offset) {
    unsigned int cm = ContextSelector::getContextForEg(offset, 0);
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

uint64_t Reader::readAsSEGbypass(unsigned int) {
    uint64_t tmp = readAsEGbypass(0);
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

uint64_t Reader::readAsSEGcabac(unsigned int, unsigned int offset) {
    uint64_t tmp = readAsEGcabac(0, offset);
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

uint64_t Reader::readAsTEGbypass(unsigned int treshold) {
    uint64_t value = readAsTUbypass(treshold);
    if (static_cast<unsigned int>(value) == treshold) {
        value += readAsEGbypass(0);
    }
    return value;
}

uint64_t Reader::readAsTEGcabac(unsigned int treshold, unsigned int offset) {
    uint64_t value = readAsTUcabac(treshold, offset);
    if (static_cast<unsigned int>(value) == treshold) {
        value += readAsEGcabac(0, offset);
    }
    return value;
}

uint64_t Reader::readAsSTEGbypass(unsigned int treshold) {
    uint64_t value = readAsTEGbypass(treshold);
    if (value != 0) {
        if (readAsBIbypass(1) == 1) {
            return uint64_t(-1 * int64_t(value));
        } else {
            return value;
        }
    }
    return value;
}

uint64_t Reader::readAsSTEGcabac(unsigned int treshold, unsigned int offset) {
    uint64_t value = readAsTEGcabac(treshold, offset);
    if (value != 0) {
        if (readAsBIcabac(1, offset) == 1) {
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

void Reader::reset() {
    m_contextModels = contexttables::buildContextTable();
    m_decBinCabac.reset();
}

}  // namespace gabac

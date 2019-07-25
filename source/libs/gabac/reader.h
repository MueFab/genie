/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_READER_H_
#define GABAC_READER_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include "bit-input-stream.h"
#include "binary-arithmetic-decoder.h"

namespace gabac {

class DataBlock;

class Reader
{
 public:
    explicit Reader(
            DataBlock *bitstream
    );

    ~Reader();

    size_t readNumSymbols();

    uint64_t readAsBIbypass(
            unsigned int cLength
    );

    uint64_t readAsBIcabac(
            unsigned int cLength,
            unsigned int offset
    );

    uint64_t readAsTUbypass(
            unsigned int cMax
    );

    uint64_t readAsTUcabac(
            unsigned int cMax,
            unsigned int offset
    );

    uint64_t readAsEGbypass(
            unsigned int dummy
    );

    uint64_t readAsEGcabac(
            unsigned int dummy,
            unsigned int offset
    );

    uint64_t readAsSEGbypass(
            unsigned int dummy
    );

    uint64_t readAsSEGcabac(
            unsigned int dummy,
            unsigned int offset
    );

    uint64_t readAsTEGbypass(
            unsigned int treshold
    );

    uint64_t readAsTEGcabac(
            unsigned int treshold,
            unsigned int offset
    );

    uint64_t readAsSTEGbypass(
            unsigned int treshold
    );

    uint64_t readAsSTEGcabac(
            unsigned int treshold,
            unsigned int offset
    );

    size_t start();

    void reset();

 private:
    BitInputStream m_bitInputStream;

    // ContextSelector m_contextSelector;

    BinaryArithmeticDecoder m_decBinCabac;

    std::vector<ContextModel> m_contextModels;
};


}  // namespace gabac


#endif  // GABAC_READER_H_

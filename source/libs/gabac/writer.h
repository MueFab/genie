/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_WRITER_H_
#define GABAC_WRITER_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include "binary-arithmetic-encoder.h"
#include "bit-output-stream.h"

namespace gabac {

class DataBlock;

class Writer {
   public:
    explicit Writer(DataBlock *bitstream);

    ~Writer();

    void start(size_t numSymbols);

    void reset();

    void writeAsBIbypass(uint64_t input, unsigned int cLength);

    void writeAsBIcabac(uint64_t input, unsigned int cLength, unsigned int offset);

    void writeAsTUbypass(uint64_t input, unsigned int cMax);

    void writeAsTUcabac(uint64_t input, unsigned int cMax, unsigned int offset);

    void writeAsEGbypass(uint64_t input, unsigned int dummy);

    void writeAsEGcabac(uint64_t input, unsigned int dummy, unsigned int offset);

    void writeAsSEGbypass(uint64_t input, unsigned int dummy);

    void writeAsSEGcabac(uint64_t input, unsigned int dummy, unsigned int offset);

    void writeAsTEGbypass(uint64_t input, unsigned int cTruncExpGolParam);

    void writeAsTEGcabac(uint64_t input, unsigned int cTruncExpGolParam, unsigned int offset);

    void writeAsSTEGbypass(uint64_t input, unsigned int cSignedTruncExpGolParam);

    void writeAsSTEGcabac(uint64_t input, unsigned int cSignedTruncExpGolParam, unsigned int offset);

    void writeNumSymbols(unsigned int numSymbols);

   private:
    BitOutputStream m_bitOutputStream;

    // ContextSelector m_contextSelector;

    BinaryArithmeticEncoder m_binaryArithmeticEncoder;

    std::vector<ContextModel> m_contextModels;
};

}  // namespace gabac

#endif  // GABAC_WRITER_H_

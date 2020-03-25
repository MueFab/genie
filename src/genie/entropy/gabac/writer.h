/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_WRITER_H_
#define GABAC_WRITER_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include <genie/util/bitwriter.h>
#include "binary-arithmetic-encoder.h"
#include "streams.h"

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

class Writer {
   public:
    explicit Writer(OBufferStream *bitstream, bool bypassFlag = true, unsigned long numContexts = 0);

    ~Writer();

    void start(size_t numSymbols);

    void close();

    void reset();

    void writeAsBIbypass(uint64_t input, unsigned int cLength);

    void writeAsBIcabac(uint64_t input, unsigned int cLength, unsigned int ctxIdx);

    void writeAsTUbypass(uint64_t input, unsigned int cMax);

    void writeAsTUcabac(uint64_t input, unsigned int cMax, unsigned int ctxIdx);

    void writeAsEGbypass(uint64_t input, unsigned int dummy);

    void writeAsEGcabac(uint64_t input, unsigned int dummy, unsigned int ctxIdx);

    void writeAsSEGbypass(uint64_t input, unsigned int dummy);

    void writeAsSEGcabac(uint64_t input, unsigned int dummy, unsigned int ctxIdx);

    void writeAsTEGbypass(uint64_t input, unsigned int cTruncExpGolParam);

    void writeAsTEGcabac(uint64_t input, unsigned int cTruncExpGolParam, unsigned int ctxIdx);

    void writeAsSTEGbypass(uint64_t input, unsigned int cSignedTruncExpGolParam);

    void writeAsSTEGcabac(uint64_t input, unsigned int cSignedTruncExpGolParam, unsigned int ctxIdx);

    void writeAsSUTUbypass(uint64_t input,
                           unsigned int outputSymSize,
                           unsigned int splitUnitSize);

    void writeAsSUTUcabac(uint64_t input,
                          unsigned int outputSymSize,
                          unsigned int splitUnitSize,
                          unsigned int ctxIdx);

    void writeAsSSUTUbypass(uint64_t input,
                            unsigned int outputSymSize,
                            unsigned int splitUnitSize);

    void writeAsSSUTUcabac(uint64_t input,
                           unsigned int outputSymSize,
                           unsigned int splitUnitSize,
                           unsigned int ctxIdx);

    void writeNumSymbols(unsigned int numSymbols);

   private:
    util::BitWriter m_bitOutputStream;

    BinaryArithmeticEncoder m_binaryArithmeticEncoder;

    bool m_bypassFlag;
    unsigned long m_numContexts;

    std::vector<ContextModel> m_contextModels;
};

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

#endif  // GABAC_WRITER_H_

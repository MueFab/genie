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
    explicit Writer(OBufferStream *bitstream, const bool bypassFlag = true, const unsigned long numContexts = 0);

    ~Writer();

    void start(const size_t numSymbols);

    void close();

    void reset();

    void writeAsBIbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsBIcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsEGcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsSEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsSEGcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsTEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsTEGcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsSTEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsSTEGcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsSUTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsSUTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsSSUTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsSSUTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsDTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsDTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeAsSDTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsSDTUcabac(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeBI(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeTU(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeEG(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeTEG(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeSUTU(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeDTU(uint64_t input, const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    void writeLutSymbol(uint64_t input, const uint8_t codingSubsymSize);

    void writeSignFlag(uint64_t input);

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

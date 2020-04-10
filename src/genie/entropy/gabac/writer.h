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

    void start();

    void close();

    void reset();

    void writeAsBIbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsBIcabac(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsTUcabac(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsEGcabac(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsTEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsTEGcabac(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsSUTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsSUTUcabac(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsDTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    void writeAsDTUcabac(uint64_t input, const std::vector<unsigned int> binParams);

    void writeBI(uint64_t input, const std::vector<unsigned int> binParams);

    void writeTU(uint64_t input, const std::vector<unsigned int> binParams);

    void writeEG(uint64_t input, const std::vector<unsigned int> binParams);

    void writeTEG(uint64_t input, const std::vector<unsigned int> binParams);

    void writeSUTU(uint64_t input, const std::vector<unsigned int> binParams);

    void writeDTU(uint64_t input, const std::vector<unsigned int> binParams);

    void writeLutSymbol(uint64_t input, const uint8_t codingSubsymSize);

    void writeSignFlag(uint64_t input);

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

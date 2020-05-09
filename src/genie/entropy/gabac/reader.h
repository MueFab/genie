/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_READER_H_
#define GABAC_READER_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include "binary-arithmetic-decoder.h"
#include "bit-input-stream.h"

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie
namespace genie {
namespace entropy {
namespace gabac {

class Reader {
   public:
    explicit Reader(util::DataBlock *bitstream, const bool bypassFlag = true, const unsigned long numContexts = 0);

    ~Reader();

    uint64_t readAsBIbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsBIcabac(const std::vector<unsigned int> binParams);

    uint64_t readAsTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsTUcabac(const std::vector<unsigned int> binParams);

    uint64_t readAsEGbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsEGcabac(const std::vector<unsigned int> binParams);

    uint64_t readAsSEGbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsSEGcabac(const std::vector<unsigned int> binParams);

    uint64_t readAsTEGbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsTEGcabac(const std::vector<unsigned int> binParams);

    uint64_t readAsSUTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsSUTUcabac(const std::vector<unsigned int> binParams);

    uint64_t readAsDTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsDTUcabac(const std::vector<unsigned int> binParams);

    uint64_t readBI(const std::vector<unsigned int> binParams);

    uint64_t readTU(const std::vector<unsigned int> binParams);

    uint64_t readEG(const std::vector<unsigned int> binParams);

    uint64_t readTEG(const std::vector<unsigned int> binParams);

    uint64_t readSUTU(const std::vector<unsigned int> binParams);

    uint64_t readDTU(const std::vector<unsigned int> binParams);

    uint64_t readLutSymbol(const uint8_t codingSubsymSize);

    bool readSignFlag();

    void start();

    size_t close();

    void reset();

   private:
    BitInputStream m_bitInputStream;
    BinaryArithmeticDecoder m_decBinCabac;

    bool m_bypassFlag;
    unsigned long m_numContexts;

    std::vector<ContextModel> m_contextModels;
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_READER_H_

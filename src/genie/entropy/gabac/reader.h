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

    size_t readNumSymbols();

    uint64_t readAsBIbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsBIcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsEGbypass(const std::vector<unsigned int> dummy);

    uint64_t readAsEGcabac(const std::vector<unsigned int> dummy, const unsigned int ctxIdx);

    uint64_t readAsSEGbypass(const std::vector<unsigned int> dummy);

    uint64_t readAsSEGcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsTEGbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsTEGcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsSTEGbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsSTEGcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsSUTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsSUTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsSSUTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsSSUTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsDTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsDTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readAsSDTUbypass(const std::vector<unsigned int> binParams);

    uint64_t readAsSDTUcabac(const std::vector<unsigned int> binParams, const unsigned int ctxIdx);

    uint64_t readLutSymbol(const uint8_t codingSubsymSize);

    size_t start();

    void close();

    void reset();

   private:
    BitInputStream m_bitInputStream;

    // ContextSelector m_contextSelector;

    BinaryArithmeticDecoder m_decBinCabac;

    bool m_bypassFlag;
    unsigned long m_numContexts;

    std::vector<ContextModel> m_contextModels;
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_READER_H_

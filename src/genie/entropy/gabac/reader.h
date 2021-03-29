/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_READER_H_
#define SRC_GENIE_ENTROPY_GABAC_READER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <vector>
#include "genie/entropy/gabac/binary-arithmetic-decoder.h"
#include "genie/entropy/gabac/bit-input-stream.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class Reader {
 public:
    /**
     *
     * @param bitstream
     * @param bypassFlag
     * @param numContexts
     */
    explicit Reader(util::DataBlock *bitstream, const bool bypassFlag = true, uint64_t numContexts = 0);

    /**
     *
     */
    ~Reader();

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsBIbypass(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsBIcabac(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsTUbypass(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsTUcabac(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsEGbypass(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsEGcabac(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsSEGbypass(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsSEGcabac(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsTEGbypass(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsTEGcabac(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsSUTUbypass(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsSUTUcabac(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsDTUbypass(const std::vector<unsigned int> binParams);

    /**
     *
     * @param binParams
     * @return
     */
    uint64_t readAsDTUcabac(const std::vector<unsigned int> binParams);

    /**
     *
     * @param codingSubsymSize
     * @return
     */
    uint64_t readLutSymbol(const uint8_t codingSubsymSize);

    /**
     *
     * @return
     */
    bool readSignFlag();

    /**
     *
     */
    void start();

    /**
     *
     * @return
     */
    size_t close();

    /**
     *
     */
    void reset();

 private:
    BitInputStream m_bitInputStream;        //!<
    BinaryArithmeticDecoder m_decBinCabac;  //!<

    bool m_bypassFlag;       //!<
    uint64_t m_numContexts;  //!<

    std::vector<ContextModel> m_contextModels;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

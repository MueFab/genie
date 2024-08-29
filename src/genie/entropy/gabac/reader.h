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

namespace genie::util {
class DataBlock;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class Reader {
 public:
    /**
     * @brief
     * @param bitstream
     * @param bypassFlag
     * @param numContexts
     */
    explicit Reader(util::DataBlock *bitstream, bool bypassFlag = true, uint64_t numContexts = 0);

    /**
     * @brief
     */
    ~Reader();

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsBIbypass(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsBIcabac(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsTUbypass(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsTUcabac(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsEGbypass(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsEGcabac(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsTEGbypass(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsTEGcabac(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsSUTUbypass(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsSUTUcabac(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsDTUbypass(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param binParams
     * @return
     */
    uint64_t readAsDTUcabac(std::vector<unsigned int> binParams);

    /**
     * @brief
     * @param codingSubsymSize
     * @return
     */
    uint64_t readLutSymbol(uint8_t codingSubsymSize);

    /**
     * @brief
     * @return
     */
    bool readSignFlag();

    /**
     * @brief
     */
    void start();

    /**
     * @brief
     * @return
     */
    size_t close();

    /**
     * @brief
     */
    void reset();

 private:
    BitInputStream m_bitInputStream;        //!< @brief
    BinaryArithmeticDecoder m_decBinCabac;  //!< @brief

    bool m_bypassFlag;       //!< @brief
    uint64_t m_numContexts;  //!< @brief

    std::vector<ContextModel> m_contextModels;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

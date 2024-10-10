/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_DECODER_H_
#define SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/gabac/bit-input-stream.h"
#include "genie/entropy/gabac/context-model.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class BinaryArithmeticDecoder {
 public:
    /**
     * @brief
     * @param bitInputStream
     */
    explicit BinaryArithmeticDecoder(const BitInputStream& bitInputStream);

    /**
     * @brief
     * @param contextModel
     * @return
     */
    unsigned int decodeBin(ContextModel* contextModel);

    /**
     * @brief
     * @param numBins
     * @return
     */
    unsigned int decodeBinsEP(unsigned int numBins);

    /**
     * @brief
     */
    void decodeBinTrm();

    /**
     * @brief
     */
    void reset();

    /**
     * @brief
     */
    void start();

    /**
     * @brief
     * @return
     */
    size_t close();

 private:
    BitInputStream m_bitInputStream;  //!< @brief
    int m_numBitsNeeded = 0;          //!< @brief
    unsigned int m_range = 0;         //!< @brief
    unsigned int m_value = 0;         //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

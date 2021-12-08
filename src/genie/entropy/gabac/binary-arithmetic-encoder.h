/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_ENCODER_H_
#define SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/gabac/context-model.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief
 */
class BinaryArithmeticEncoder {
 public:
    /**
     * @brief
     * @param bitOutputStream
     */
    explicit BinaryArithmeticEncoder(const util::BitWriter& bitOutputStream);

    /**
     * @brief
     */
    ~BinaryArithmeticEncoder();

    /**
     * @brief
     * @param bin
     * @param contextModel
     */
    void encodeBin(unsigned int bin, ContextModel* contextModel);

    /**
     * @brief
     * @param bin
     */
    void encodeBinEP(unsigned int bin);

    /**
     * @brief
     * @param bins
     * @param numBins
     */
    void encodeBinsEP(unsigned int bins, unsigned int numBins);

    /**
     * @brief
     * @param bin
     */
    void encodeBinTrm(unsigned int bin);

    /**
     * @brief
     */
    void start();

    /**
     * @brief
     */
    void flush();

 private:
    /**
     * @brief
     */
    void finish();

    /**
     * @brief
     */
    void writeOut();

    util::BitWriter m_bitOutputStream;  //!< @brief

    unsigned char m_bufferedByte;  //!< @brief

    unsigned int m_low;  //!< @brief

    unsigned int m_numBitsLeft;  //!< @brief

    int m_numBufferedBytes;  //!< @brief

    unsigned int m_range;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

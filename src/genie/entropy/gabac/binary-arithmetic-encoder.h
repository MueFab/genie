/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_BINARY_ARITHMETIC_ENCODER_H_
#define GABAC_BINARY_ARITHMETIC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include "context-model.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class BinaryArithmeticEncoder {
   public:
    /**
     *
     * @param bitOutputStream
     */
    explicit BinaryArithmeticEncoder(const util::BitWriter& bitOutputStream);

    /**
     *
     */
    ~BinaryArithmeticEncoder();

    /**
     *
     * @param bin
     * @param contextModel
     */
    void encodeBin(unsigned int bin, ContextModel* contextModel);

    /**
     *
     * @param bin
     */
    void encodeBinEP(unsigned int bin);

    /**
     *
     * @param bins
     * @param numBins
     */
    void encodeBinsEP(unsigned int bins, unsigned int numBins);

    /**
     *
     * @param bin
     */
    void encodeBinTrm(unsigned int bin);

    /**
     *
     */
    void start();

    /**
     *
     */
    void flush();

   private:
    /**
     *
     */
    void finish();

    /**
     *
     */
    void writeOut();

    util::BitWriter m_bitOutputStream;  //!<

    unsigned char m_bufferedByte;  //!<

    unsigned int m_low;  //!<

    unsigned int m_numBitsLeft;  //!<

    int m_numBufferedBytes;  //!<

    unsigned int m_range;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_BINARY_ARITHMETIC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_BINARY_ARITHMETIC_DECODER_H_
#define GABAC_BINARY_ARITHMETIC_DECODER_H_

#include "bit-input-stream.h"

namespace genie {
namespace gabac {

class ContextModel;

class BinaryArithmeticDecoder {
   public:
    explicit BinaryArithmeticDecoder(const BitInputStream& bitInputStream);
    unsigned int decodeBin(ContextModel* contextModel);
    unsigned int decodeBinsEP(unsigned int numBins);
    void decodeBinTrm();
    void reset();
    void start();

   private:
    BitInputStream m_bitInputStream;
    int m_numBitsNeeded = 0;
    unsigned int m_range = 0;
    unsigned int m_value = 0;
};

}  // namespace gabac
}  // namespace genie

#endif  // GABAC_BINARY_ARITHMETIC_DECODER_H_

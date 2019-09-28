/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_BINARY_ARITHMETIC_ENCODER_H_
#define GABAC_BINARY_ARITHMETIC_ENCODER_H_

#include "util/bitwriter.h"

namespace gabac {

class ContextModel;

class BinaryArithmeticEncoder {
   public:
    explicit BinaryArithmeticEncoder(const util::BitWriter& bitOutputStream);

    ~BinaryArithmeticEncoder();

    void encodeBin(unsigned int bin, ContextModel* contextModel);

    void encodeBinEP(unsigned int bin);

    void encodeBinsEP(unsigned int bins, unsigned int numBins);

    void encodeBinTrm(unsigned int bin);

    void start();

    void flush();

   private:
    void finish();

    void writeOut();

    util::BitWriter m_bitOutputStream;

    unsigned char m_bufferedByte;

    unsigned int m_low;

    unsigned int m_numBitsLeft;

    int m_numBufferedBytes;

    unsigned int m_range;
};

}  // namespace gabac

#endif  // GABAC_BINARY_ARITHMETIC_ENCODER_H_

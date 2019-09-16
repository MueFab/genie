/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_BIT_OUTPUT_STREAM_H_
#define GABAC_BIT_OUTPUT_STREAM_H_

namespace gabac {

class DataBlock;

class BitOutputStream {
   public:
    explicit BitOutputStream(DataBlock *bitstream);

    ~BitOutputStream();

    void flush();

    void write(unsigned int bits, unsigned int numBits);

    void writeAlignZero();

   private:
    DataBlock *m_bitstream;

    unsigned char m_heldBits;

    unsigned int m_numHeldBits;
};

}  // namespace gabac

#endif  // GABAC_BIT_OUTPUT_STREAM_H_

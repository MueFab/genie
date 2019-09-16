/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_BIT_INPUT_STREAM_H_
#define GABAC_BIT_INPUT_STREAM_H_

#include "block_stepper.h"

namespace gabac {

class DataBlock;

class BitInputStream {
   public:
    explicit BitInputStream(DataBlock *bitstream);

    ~BitInputStream();

    unsigned int getNumBitsUntilByteAligned() const;

    unsigned char readByte();

    void reset();

    // private:
    unsigned int read(unsigned int numBits);

   private:  // i added this
    gabac::DataBlock *m_bitstream;

    gabac::BlockStepper m_reader;

    unsigned char m_heldBits;

    unsigned int m_numHeldBits;
};

}  // namespace gabac

#endif  // GABAC_BIT_INPUT_STREAM_H_

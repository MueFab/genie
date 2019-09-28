/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_BIT_INPUT_STREAM_H_
#define GABAC_BIT_INPUT_STREAM_H_

#include "block-stepper.h"

namespace gabac {

class DataBlock;

class BitInputStream {
   public:
    explicit BitInputStream(DataBlock *bitstream);

    ~BitInputStream();

    unsigned int getNumBitsUntilByteAligned() const;

    unsigned char readByte();

    unsigned int read(unsigned int numBits);

    void reset();

   private:
    gabac::DataBlock *m_bitstream;

    gabac::BlockStepper m_reader;

    unsigned char m_heldBits;

    unsigned int m_numHeldBits;
};

}  // namespace gabac

#endif  // GABAC_BIT_INPUT_STREAM_H_

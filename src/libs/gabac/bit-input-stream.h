/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_BIT_INPUT_STREAM_H_
#define GABAC_BIT_INPUT_STREAM_H_

#include <util/block-stepper.h>
#include <util/data-block.h>

namespace util {
class DataBlock;
}

namespace genie {
namespace gabac {

class BitInputStream {
   public:
    explicit BitInputStream(util::DataBlock *bitstream);

    ~BitInputStream();

    unsigned int getNumBitsUntilByteAligned() const;

    unsigned char readByte();

    unsigned int read(unsigned int numBits);

    void skipBytes(unsigned int numytes);

    void reset();

   private:
    util::DataBlock *m_bitstream;

    util::BlockStepper m_reader;

    unsigned char m_heldBits;

    unsigned int m_numHeldBits;
};
}  // namespace gabac
}  // namespace genie

#endif  // GABAC_BIT_INPUT_STREAM_H_

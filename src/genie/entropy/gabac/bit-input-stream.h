/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_BIT_INPUT_STREAM_H_
#define GABAC_BIT_INPUT_STREAM_H_

#include <genie/util/block-stepper.h>
#include <genie/util/data-block.h>

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
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
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_BIT_INPUT_STREAM_H_

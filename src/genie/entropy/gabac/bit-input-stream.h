/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BIT_INPUT_STREAM_H_
#define SRC_GENIE_ENTROPY_GABAC_BIT_INPUT_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/block-stepper.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class BitInputStream {
 public:
    /**
     *
     * @param bitstream
     */
    explicit BitInputStream(util::DataBlock *bitstream);

    /**
     *
     */
    ~BitInputStream();

    /**
     *
     * @return
     */
    unsigned int getNumBitsUntilByteAligned() const;

    /**
     *
     * @return
     */
    size_t getNumBytesRead() const;

    /**
     *
     * @return
     */
    unsigned char readByte();

    /**
     *
     * @param numBits
     * @return
     */
    unsigned int read(unsigned int numBits);

    /**
     *
     * @param numytes
     */
    void skipBytes(unsigned int numytes);

    /**
     *
     */
    void reset();

 private:
    util::DataBlock *m_bitstream;  //!<

    util::BlockStepper m_reader;  //!<

    unsigned char m_heldBits;  //!<

    unsigned int m_numHeldBits;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BIT_INPUT_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

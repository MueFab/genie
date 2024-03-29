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
 * @brief
 */
class BitInputStream {
 public:
    /**
     * @brief
     * @param bitstream
     */
    explicit BitInputStream(util::DataBlock *bitstream);

    /**
     * @brief
     */
    ~BitInputStream();

    /**
     * @brief
     * @return
     */
    unsigned int getNumBitsUntilByteAligned() const;

    /**
     * @brief
     * @return
     */
    size_t getNumBytesRead() const;

    /**
     * @brief
     * @return
     */
    unsigned char readByte();

    /**
     * @brief
     * @param numBits
     * @return
     */
    unsigned int read(unsigned int numBits);

    /**
     * @brief
     * @param numytes
     */
    void skipBytes(unsigned int numytes);

    /**
     * @brief
     */
    void reset();

 private:
    util::DataBlock *m_bitstream;  //!< @brief

    util::BlockStepper m_reader;  //!< @brief

    unsigned char m_heldBits;  //!< @brief

    unsigned int m_numHeldBits;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BIT_INPUT_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

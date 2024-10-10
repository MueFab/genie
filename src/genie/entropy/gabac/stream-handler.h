/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_STREAM_HANDLER_H_
#define SRC_GENIE_ENTROPY_GABAC_STREAM_HANDLER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <istream>
#include <ostream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {
class DataBlock;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class StreamHandler {
 public:
    /**
     * @brief
     * @param input
     * @param retVal
     * @param numBytes
     * @return
     */
    static size_t readUInt(std::istream& input, uint64_t& retVal, size_t numBytes);

    /**
     * @brief
     * @param input
     * @param retVal
     * @return
     */
    static size_t readU7(std::istream& input, uint64_t& retVal);

    /**
     * @brief
     * @param input
     * @param buffer
     * @param numSymbols
     * @return
     */
    static size_t readStream(std::istream& input, util::DataBlock* buffer, uint64_t& numSymbols);

    /**
     * @brief
     * @param input
     * @param bytes
     * @param buffer
     * @return
     */
    static size_t readBytes(std::istream& input, size_t bytes, util::DataBlock* buffer);

    /**
     * @brief
     * @param input
     * @param buffer
     * @return
     */
    static size_t readFull(std::istream& input, util::DataBlock* buffer);

    /**
     * @brief
     * @param input
     * @param bytes
     * @param buffer
     * @return
     */
    static size_t readBlock(std::istream& input, size_t bytes, util::DataBlock* buffer);

    /**
     * @brief
     * @param input
     * @return
     */
    static size_t readStreamSize(std::istream& input);

    /**
     * @brief
     * @param output
     * @param value
     * @param numBytes
     * @return
     */
    static size_t writeUInt(std::ostream& output, uint64_t value, size_t numBytes);

    /**
     * @brief
     * @param output
     * @param value
     * @return
     */
    static size_t writeU7(std::ostream& output, uint64_t value);

    /**
     * @brief
     * @param output
     * @param buffer
     * @param numSymbols
     * @return
     */
    static size_t writeStream(std::ostream& output, util::DataBlock* buffer, uint64_t numSymbols);

    /**
     * @brief
     * @param output
     * @param buffer
     * @return
     */
    static size_t writeBytes(std::ostream& output, util::DataBlock* buffer);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_STREAM_HANDLER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

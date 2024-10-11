/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 * @brief Declaration of the StreamHandler utility class for managing stream operations.
 *
 * This file defines the StreamHandler class, which provides static methods for reading and writing
 * various types of data to and from streams, such as unsigned integers, symbols, and custom data blocks.
 * The class handles different data formats and sizes, ensuring proper handling of bit-level and byte-level operations.
 *
 * @details The StreamHandler class offers various functionalities, including reading and writing specific
 * amounts of data, handling byte-aligned reads, encoding and decoding U7 values, and managing the conversion
 * of stream sizes. Each method is designed to operate on input and output streams, making it a versatile
 * utility for handling encoded data in GENIE.
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
}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Utility class for reading and writing encoded data to and from streams.
 *
 * The StreamHandler class provides static methods for reading and writing different
 * data types to and from input and output streams. It includes functionality for handling
 * unsigned integers of various sizes, reading and writing blocks of symbols, and managing
 * custom data formats like U7-encoded values. The class operates primarily on `std::istream`
 * and `std::ostream` objects.
 */
class StreamHandler {
 public:
    /**
     * @brief Reads an unsigned integer from the input stream with a specified number of bytes.
     *
     * This function reads the requested number of bytes from the input stream and interprets
     * them as an unsigned integer, storing the result in `retVal`.
     *
     * @param input The input stream to read from.
     * @param retVal Reference to store the read value.
     * @param numBytes Number of bytes to read from the stream.
     * @return The number of bytes successfully read.
     */
    static size_t readUInt(std::istream& input, uint64_t& retVal, size_t numBytes);

    /**
     * @brief Reads a U7-encoded value from the input stream.
     *
     * This function reads a U7-encoded value (an unsigned 7-bit integer) from the input
     * stream and stores it in `retVal`.
     *
     * @param input The input stream to read from.
     * @param retVal Reference to store the read U7 value.
     * @return The number of bytes successfully read.
     */
    static size_t readU7(std::istream& input, uint64_t& retVal);

    /**
     * @brief Reads a stream of symbols into a buffer.
     *
     * Reads a sequence of symbols from the input stream and stores them in the provided
     * `buffer`. The number of symbols read is stored in `numSymbols`.
     *
     * @param input The input stream to read from.
     * @param buffer Pointer to a DataBlock to store the read symbols.
     * @param numSymbols Reference to store the number of symbols read.
     * @return The number of bytes successfully read.
     */
    static size_t readStream(std::istream& input, util::DataBlock* buffer, uint64_t& numSymbols);

    /**
     * @brief Reads a specified number of bytes into a buffer.
     *
     * Reads the specified number of bytes from the input stream and stores them in
     * the given `buffer`.
     *
     * @param input The input stream to read from.
     * @param bytes The number of bytes to read.
     * @param buffer Pointer to a DataBlock to store the read bytes.
     * @return The number of bytes successfully read.
     */
    static size_t readBytes(std::istream& input, size_t bytes, util::DataBlock* buffer);

    /**
     * @brief Reads the entire content of the input stream into a buffer.
     *
     * Reads all the bytes available in the input stream and stores them in the provided
     * `buffer`. Useful for reading the complete contents of a file or stream.
     *
     * @param input The input stream to read from.
     * @param buffer Pointer to a DataBlock to store the read data.
     * @return The number of bytes successfully read.
     */
    static size_t readFull(std::istream& input, util::DataBlock* buffer);

    /**
     * @brief Reads a block of specified size from the input stream.
     *
     * Reads `bytes` number of bytes from the input stream and stores them in the provided
     * `buffer`.
     *
     * @param input The input stream to read from.
     * @param bytes The number of bytes to read.
     * @param buffer Pointer to a DataBlock to store the read data.
     * @return The number of bytes successfully read.
     */
    static size_t readBlock(std::istream& input, size_t bytes, util::DataBlock* buffer);

    /**
     * @brief Reads the size of the stream in bytes.
     *
     * Reads and returns the size of the input stream by determining the number of bytes
     * available in the stream.
     *
     * @param input The input stream to analyze.
     * @return The size of the stream in bytes.
     */
    static size_t readStreamSize(std::istream& input);

    /**
     * @brief Writes an unsigned integer to the output stream using a specified number of bytes.
     *
     * Writes `value` to the output stream using `numBytes` bytes.
     *
     * @param output The output stream to write to.
     * @param value The unsigned integer to write.
     * @param numBytes The number of bytes to write the value with.
     * @return The number of bytes successfully written.
     */
    static size_t writeUInt(std::ostream& output, uint64_t value, size_t numBytes);

    /**
     * @brief Writes a U7-encoded value to the output stream.
     *
     * This function encodes the given `value` using the U7 format and writes it to the
     * output stream.
     *
     * @param output The output stream to write to.
     * @param value The U7-encoded value to write.
     * @return The number of bytes successfully written.
     */
    static size_t writeU7(std::ostream& output, uint64_t value);

    /**
     * @brief Writes a stream of symbols to the output stream.
     *
     * Writes the content of `buffer` to the output stream as a sequence of symbols.
     * The number of symbols to be written is specified by `numSymbols`.
     *
     * @param output The output stream to write to.
     * @param buffer Pointer to a DataBlock containing the symbols to be written.
     * @param numSymbols The number of symbols to write.
     * @return The number of bytes successfully written.
     */
    static size_t writeStream(std::ostream& output, util::DataBlock* buffer, uint64_t numSymbols);

    /**
     * @brief Writes a sequence of bytes to the output stream.
     *
     * Writes the contents of `buffer` to the output stream as a sequence of bytes.
     *
     * @param output The output stream to write to.
     * @param buffer Pointer to a DataBlock containing the bytes to be written.
     * @return The number of bytes successfully written.
     */
    static size_t writeBytes(std::ostream& output, util::DataBlock* buffer);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_STREAM_HANDLER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

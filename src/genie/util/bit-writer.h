/**
 * @file bit-writer.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/mitogen/genie for more details.
 *
 * @brief Declaration of the BitWriter utility class for writing bits to a stream.
 *
 * This file contains the declaration of the BitWriter class, which provides methods
 * to write bits and bytes to an output stream. It supports both non-aligned and byte-aligned
 * writing operations.
 *
 * @details The BitWriter class offers functionalities like writing a specific number of bits,
 * flushing bits, checking byte alignment, writing bytes, writing aligned streams, and manipulating
 * the output stream position.
 */

#ifndef SRC_GENIE_UTIL_BIT_WRITER_H_
#define SRC_GENIE_UTIL_BIT_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <istream>
#include <ostream>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Controlled output to a std::ostream. Allows to write single bits.
 */
class BitWriter {
 private:
    std::ostream &stream;       //!< @brief Where to direct output. A file for example.
    uint64_t heldBits;          //!< @brief Contains bits which cannot be written yet, as no byte is full.
    uint8_t numHeldBits;        //!< @brief How many bits there are in heldBits.
    uint64_t totalBitsWritten;  //!< @brief Counts number of written bits for statistical usages.

 public:
    /**
     * @brief Constructs a BitWriter and initializes the output stream.
     *
     * @param str Pointer to an output stream (std::ostream) where bits will be written.
     */
    explicit BitWriter(std::ostream &str);

    /**
     * @brief Destructor for the BitWriter class.
     *
     * Ensures that any remaining bits held in the internal buffer are written
     * to the associated output stream before the BitWriter object is destroyed.
     */
    ~BitWriter();

    // Non Byte-Aligned Writing ----------------------------------------------------------------------------------------

    /**
     * @brief Writes the specified number of bits from the given value to the output stream.
     *
     * This method handles the bit-level alignment ensuring that if there are any bits currently held,
     * they are concatenated with the new bits and written out appropriately.
     *
     * @param value The value containing the bits to write.
     * @param bits The number of bits to write from the provided value.
     */
    void writeBits(uint64_t value, uint8_t bits);

    /**
     * @brief Flushes any remaining bits that are currently held and writes them to the output stream.
     *
     * This method checks if there are any bits currently held (i.e., if numHeldBits is greater than 0).
     * If there are held bits, it writes these bits to the stream by calling the writeBits method.
     * Afterward, it resets the held bits (heldBits) and the count of held bits (numHeldBits) to 0.
     */
    void flushBits();

    /**
     * @brief Retrieves the total number of bits written to the output stream.
     *
     * This function returns the cumulative count of bits that have been written
     * to the output stream, including any bits that are currently held.
     *
     * @return The total number of bits written.
     */
    [[nodiscard]] uint64_t getTotalBitsWritten() const;

    // Byte-Aligned Writing --------------------------------------------------------------------------------------------

    /**
     * @brief Checks if the current position is byte-aligned.
     *
     * Determines whether the number of held bits is zero, implying that
     * the current position aligns with a byte boundary.
     *
     * @return true if the current position is byte-aligned.
     * @return false otherwise.
     */
    [[nodiscard]] bool isByteAligned() const;

    /**
     * @brief Writes an entire byte to the output stream aligned with byte boundaries.
     *
     * This function writes a single byte to the associated output stream. It ensures
     * that the byte written is byte-aligned, meaning that any pending partial bytes
     * from previous operations are handled appropriately.
     *
     * @param byte The byte value to be written to the output stream.
     */
    void writeAlignedByte(uint8_t byte);

    /**
     * @brief Writes a specified number of bytes to the output stream in an aligned manner.
     *
     * This function writes exactly `size` bytes from the input buffer `in` to the
     * underlying output stream. The bytes are written only if the current bit
     * position is byte-aligned, ensuring the correct byte-boundary writing.
     *
     * @param in Pointer to the input buffer containing bytes to be written.
     * @param size Number of bytes to write from the input buffer.
     *
     * @throws If the current bit position is not byte-aligned.
     */
    void writeAlignedBytes(const void *in, size_t size);

    /**
     * @brief Writes a specified number of bytes from the provided bits to the output stream in a byte-aligned manner.
     *
     * This function breaks down the given `writeBits` into `numBytes` and writes these bytes to the output stream,
     * ensuring each byte is written in a byte-aligned manner. It uses a series of labels for efficient byte-writing.
     *
     * @param writeBits The bits to be written as bytes to the output stream.
     * @param numBytes The number of bytes to take from `writeBits` and write to the output stream.
     */
    inline void writeAlignedBytes(uint64_t writeBits, uint8_t numBytes);

    /**
     * @brief Writes an aligned integral value to the associated output stream.
     *
     * This template function ensures that the provided integral value is written
     * in a manner that aligns to the specified size.
     *
     * @tparam T The type of the integral value to be written. Must be an integral type.
     * @tparam NUM_BYTES The size to align the value to. Defaults to the size of the type T.
     * @param val The integral value to be written to the output stream.
     */
    template <typename T, size_t NUM_BYTES = sizeof(T), typename = std::enable_if<std::is_integral<T>::value>>
    void writeAlignedInt(T val);

    /**
     * @brief Writes the contents of an input stream to the output stream in byte-aligned mode.
     *
     * This method reads data from the provided input stream and writes it to the output stream in chunks,
     * ensuring that the writer is byte-aligned before proceeding with the write operation. It continues
     * to read and write data in chunks of predefined buffer size until the entire input stream is processed.
     *
     * @param in Reference to the input stream to be read and written.
     *
     * @pre The BitWriter must be byte-aligned before calling this method.
     * @throw runtime_error if the writer is not byte-aligned before writing.
     */
    void writeAlignedStream(std::istream &in);

    // Stream Manipulation ---------------------------------------------------------------------------------------------

    /**
     * @brief Retrieves the current position in the stream.
     *
     * @return The current stream position in bytes.
     */
    [[nodiscard]] int64_t getStreamPosition() const;

    /**
     * Sets the position of the stream to the specified position.
     *
     * @param pos The position to which the stream should be moved.
     */
    void setStreamPosition(int64_t pos) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit-writer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BIT_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

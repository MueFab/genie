/**
 * Copyright 2018-2024 The Genie Authors.
 * @file bit_writer.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the BitWriter utility class for writing bits to a
 * stream.
 *
 * This file contains the declaration of the BitWriter class, which provides
 * methods to write bits and bytes to an output stream. It supports both
 * non-aligned and byte-aligned writing operations.
 *
 * @details The BitWriter class offers functionalities like writing a specific
 * number of bits, flushing bits, checking byte alignment, writing bytes,
 * writing aligned streams, and manipulating the output stream position.
 */

#ifndef SRC_GENIE_UTIL_BIT_WRITER_H_
#define SRC_GENIE_UTIL_BIT_WRITER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <ostream>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Controlled output to a std::ostream. Allows to write single bits.
 */
class BitWriter {
  /// Where to direct output. A file for example.
  std::ostream& stream_;

  /// Contains bits which cannot be written yet, as no byte is full.
  uint64_t held_bits_;

  /// How many bits there are in heldBits.
  uint8_t num_held_bits_;

  /// Counts number of written bits for statistical usages.
  uint64_t total_bits_written_;

 public:
  /**
   * @brief Constructs a BitWriter and initializes the output stream.
   *
   * @param str Pointer to an output stream (std::ostream) where bits will be
   * written.
   */
  explicit BitWriter(std::ostream& str);

  /**
   * @brief Destructor for the BitWriter class.
   *
   * Ensures that any remaining bits held in the internal buffer are written
   * to the associated output stream before the BitWriter object is destroyed.
   */
  ~BitWriter();

  // Non Byte-Aligned Writing
  // -------------------------------------------------------------------------

  /**
   * @brief Writes the specified number of bits from the given value to the
   * output stream.
   *
   * This method handles the bit-level alignment ensuring that if there are
   * any bits currently held, they are concatenated with the new bits and
   * written out appropriately.
   *
   * @param value The value containing the bits to write.
   * @param bits The number of bits to write from the provided value.
   */
  void WriteBits(uint64_t value, uint8_t bits);

  /**
   * @brief Flushes any remaining bits that are currently held and writes them
   * to the output stream.
   *
   * This method checks if there are any bits currently held (i.e., if
   * numHeldBits is greater than 0). If there are held bits, it writes these
   * bits to the stream by calling the writeBits method. Afterward, it resets
   * the held bits (heldBits) and the count of held bits (numHeldBits) to 0.
   */
  void FlushBits();

  /**
   * @brief Retrieves the total number of bits written to the output stream.
   *
   * This function returns the cumulative count of bits that have been written
   * to the output stream, including any bits that are currently held.
   *
   * @return The total number of bits written.
   */
  [[nodiscard]] uint64_t GetTotalBitsWritten() const;

  // Byte-Aligned Writing
  // -------------------------------------------------------------------------

  /**
   * @brief Checks if the current position is byte-aligned.
   *
   * Determines whether the number of held bits is zero, implying that
   * the current position aligns with a byte boundary.
   *
   * @return true if the current position is byte-aligned.
   * @return false otherwise.
   */
  [[nodiscard]] bool IsByteAligned() const;

  /**
   * @brief Writes an entire byte to the output stream aligned with byte
   * boundaries.
   *
   * This function writes a single byte to the associated output stream. It
   * ensures that the byte written is byte-aligned, meaning that any pending
   * partial bytes from previous operations are handled appropriately.
   *
   * @param byte The byte value to be written to the output stream.
   */
  void WriteAlignedByte(uint8_t byte);

  /**
   * @brief Writes a specified number of bytes to the output stream in an
   * aligned manner.
   *
   * This function writes exactly `Size` bytes from the input buffer `in` to
   * the underlying output stream. The bytes are written only if the current
   * bit position is byte-aligned, ensuring the correct byte-boundary writing.
   *
   * @param in Pointer to the input buffer containing bytes to be written.
   * @param size Number of bytes to write from the input buffer.
   *
   * @throws If the current bit position is not byte-aligned.
   */
  void WriteAlignedBytes(const void* in, size_t size);

  /**
   * @brief Writes a specified number of bytes from the provided bits to the
   * output stream in a byte-aligned manner.
   *
   * This function breaks down the given `writeBits` into `numBytes` and
   * writes these bytes to the output stream, ensuring each byte is written in
   * a byte-aligned manner. It uses a series of labels for efficient
   * byte-writing.
   *
   * @param write_bits The bits to be written as bytes to the output stream.
   * @param num_bytes The number of bytes to take from `writeBits` and write
   * to the output stream.
   */
  inline void WriteAlignedBytes(uint64_t write_bits, uint8_t num_bytes);

  /**
   * @brief Writes an aligned integral value to the associated output stream.
   *
   * This template function ensures that the provided integral value is
   * written in a manner that aligns to the specified Size.
   *
   * @tparam T The type of the integral value to be written. Must be an
   * integral type.
   * @tparam NumBytes The size to align the value to. Defaults to the Size of
   * the type T.
   * @param val The integral value to be written to the output stream.
   */
  template <typename T, size_t NumBytes = sizeof(T),
            typename = std::enable_if<std::is_integral_v<T>>>
  void WriteAlignedInt(T val);

  /**
   * @brief Writes the contents of an input stream to the output stream in
   * byte-aligned mode.
   *
   * This method reads data from the provided input stream and writes it to
   * the output stream in chunks, ensuring that the writer is byte-aligned
   * before proceeding with the write operation. It continues to read and
   * write data in chunks of predefined buffer Size until the entire input
   * stream is processed.
   *
   * @param in Reference to the input stream to be read and written.
   *
   * @pre The BitWriter must be byte-aligned before calling this method.
   * @throw runtime_error if the writer is not byte-aligned before writing.
   */
  void WriteAlignedStream(std::istream& in);

  // Stream Manipulation
  // -------------------------------------------------------------------------

  /**
   * @brief Retrieves the current position in the stream.
   *
   * @return The current stream position in bytes.
   */
  [[nodiscard]] int64_t GetStreamPosition() const;

  /**
   * Sets the position of the stream to the specified position.
   *
   * @param pos The position to which the stream should be moved.
   */
  void SetStreamPosition(int64_t pos) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/bit_writer.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BIT_WRITER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

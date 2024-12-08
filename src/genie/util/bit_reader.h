/**
 * Copyright 2018-2024 The Genie Authors.
 * @file bit_reader.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the BitReader utility class for reading bits from a
 * stream.
 *
 * This file contains the declaration of the BitReader class, which provides
 * methods to read bits and bytes from an input stream. It supports both
 * non-aligned and byte-aligned reading operations.
 *
 * @details The BitReader class offers functionalities like reading a specific
 * number of bits, flushing bits, checking byte alignment, reading bytes,
 * reading and skipping aligned strings, and manipulating the input stream
 * position.
 */

#ifndef SRC_GENIE_UTIL_BIT_READER_H_
#define SRC_GENIE_UTIL_BIT_READER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <istream>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Wrapper around an input stream to read data bit by bit instead of byte
 * aligned.
 */
class BitReader {
 public:
  /**
   * @brief Constructs a BitReader object from an input stream.
   *
   * This constructor initializes a BitReader, associating it with the
   * provided input stream. It is designed to read data bit by bit from the
   * stream rather than in a byte-aligned manner.
   *
   * @param istream A reference to an input stream object.
   */
  explicit BitReader(std::istream& istream);

  // Non Byte-Aligned reading
  // -------------------------------------------------------------------------

  /**
   * @brief Reads the specified number of bits from the input stream.
   *
   * This function reads `numBits` bits from the current position in the
   * input stream, updating the internal state to track the number of bits
   * read. If the requested number of bits is less than or equal to the number
   * of bits currently held, it extracts the bits directly from the held bits.
   * Otherwise, it loads additional bytes from the input stream to satisfy
   * the request.
   *
   * @param num_bits The number of bits to read.
   * @return uint64_t The value of the read bits.
   */
  uint64_t ReadBits(uint8_t num_bits);

  /**
   * @brief Retrieves the total number of bits read so far.
   *
   * @return The number of bits read from the stream up to the current point.
   */
  [[nodiscard]] uint64_t GetTotalBitsRead() const;

  /**
   * @brief Reads and returns an integral type value from the bit stream.
   *
   * This function reads a number of bits from the bit stream equivalent to
   * the Size of the integral type T in bytes multiplied by 8. It is meant to
   * be used with integral types, enforced by the std::enable_if condition.
   *
   * @tparam T Integral type to be read from the bit stream.
   *
   * @return The value of type T read from the bit stream.
   */
  template <typename T, typename = std::enable_if<std::is_integral_v<T>>>
  T Read();

  /**
   * @brief Reads the specified number of bits and returns them as an integral
   * type.
   *
   * This function reads `numBits` bits from the input stream, interprets them
   * as an unsigned integer, and casts them to the specified integral type
   * `T`.
   *
   * @tparam T The integral type that the read bits will be cast to.
   * @param num_bits The number of bits to read from the input stream.
   * @return The value of the read bits cast to type `T`.
   */
  template <typename T, typename = std::enable_if<std::is_integral_v<T>>>
  T Read(uint8_t num_bits);

  /**
   * @brief Clears the currently held bits and returns them.
   *
   * This function resets the internal variables `heldBits` and `numHeldBits`
   * to 0, indicating that there are no more bits currently held. It also
   * updates the `totalBitsRead` counter to include the number of bits that
   * were held.
   *
   * @return The bits that were currently held before they were cleared.
   */
  uint64_t FlushHeldBits();

  // Byte-Aligned Reading
  // -------------------------------------------------------------------------

  /**
   * @brief Checks if the current reading position is byte-aligned.
   *
   * This function returns true if the reader is currently aligned to a byte
   * boundary. A byte is aligned if no bits are being held from a previous
   * byte.
   *
   * @return true if the reader is byte-aligned (i.e., no held bits), false
   * otherwise.
   */
  [[nodiscard]] bool IsByteAligned() const;

  /**
   * @brief Reads a single byte from the input stream.
   *
   * This function reads a single byte from the associated input stream,
   * ensuring that the read is byte-aligned.
   *
   * @return The byte read from the stream as an uint64_t value.
   */
  [[nodiscard]] uint64_t ReadAlignedByte() const;

  /**
   * @brief Reads a string from the input stream, where the string is
   * terminated by a null character (\0).
   *
   * @return The string read from the input stream.
   *
   * This function reads characters from the input stream until a null
   * character is encountered. The characters are stored in a string and the
   * length of the string is multiplied by BITS_PER_BYTE to update the total
   * number of bits read.
   */
  [[nodiscard]] std::string ReadAlignedStringTerminated();

  /**
   * @brief Reads a specified number of bytes from the input stream without
   * considering the bit alignment.
   *
   * This function reads `Size` bytes from the `input_stream` associated with
   * the `BitReader` and stores them into the memory location pointed to by
   * `in`. It also updates the `totalBitsRead` member to reflect the number of
   * bits read.
   *
   * @param in Pointer to a memory location where the read bytes will be
   * stored.
   * @param size Number of bytes to read from the input stream.
   */
  void ReadAlignedBytes(void* in, size_t size);

  /**
   * @brief Skips a specified number of bytes in the input stream.
   *
   * This function skips the next 'bytes' bytes in the input stream
   * associated with this BitReader instance. Useful for seeking forward
   * in the stream without reading and processing the data.
   *
   * @param bytes The number of bytes to skip.
   */
  void SkipAlignedBytes(size_t bytes) const;

  /**
   * @brief Reads an aligned big-endian value from the input stream.
   *
   * This function reads a value of the specified type `T` from the current
   * position of the input stream associated with the `BitReader` instance. It
   * reads a number of bytes equal to `NUM_BYTES`, which defaults to the Size
   * of the type `T`. The value is assumed to be in big-endian format and will
   * be converted to the host's endianness if necessary.
   *
   * @tparam T The type of the value to read from the input stream. Must be an
   * integral type.
   * @tparam NumBytes The number of bytes to read. Defaults to the Size of
   * type `T`.
   * @return The value read from the input stream, converted to the host's
   * endianness.
   */
  template <typename T, size_t NumBytes = sizeof(T),
            typename = std::enable_if<std::is_integral_v<T>>>
  T ReadAlignedInt();

  // Input stream manipulation
  // -------------------------------------------------------------------------

  /**
   * @brief Retrieves the current position of the input stream.
   * @return The current position in the input stream as an integer of type
   * int64_t.
   */
  [[nodiscard]] uint64_t GetStreamPosition() const;

  /**
   * @brief Sets the position of the input stream to the specified position.
   *
   * This function sets the position of the underlying input stream
   * (`input_stream`) to the provided `pos` value as an absolute position
   * from the beginning of the stream.
   *
   * @param pos The position to which the stream should be set, in bytes.
   */
  void SetStreamPosition(uint64_t pos) const;

  /**
   * @brief Clears the state of the associated input stream.
   *
   * This function will Clear any error flags that may have been
   * set on the input stream associated with the BitReader.
   */
  void ClearStreamState() const;

  /**
   * @brief Checks the state of the input stream.
   *
   * This function evaluates the current state of the input stream that the
   * BitReader is managing. It returns true if the stream is in a good state
   * and false if the stream has encountered any errors.
   *
   * @return True if the input stream is in a good state, false otherwise.
   */
  [[nodiscard]] bool IsStreamGood() const;

 private:
  std::istream& input_stream_;
  uint8_t held_bits_;
  uint8_t num_held_bits_;
  uint64_t total_bits_read_;

  /**
   * Loads an aligned word from the bit stream with the specified number of
   * bits.
   *
   * This function reads the required number of aligned bytes (based on the
   * specified number of bits) from the bit stream and constructs a 64-bit
   * word. The number of bytes to be loaded is determined by `numBits`, which
   * is the number of bits to be loaded. The function efficiently handles
   * loading between 1 and 8 bytes.
   *
   * @param num_bits The number of bits to be loaded into the word. Valid
   * values range from 1 to 64.
   * @return A 64-bit word containing the loaded bits from the stream.
   */
  [[nodiscard]] inline uint64_t LoadAlignedWord(uint8_t num_bits) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/bit_reader.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BIT_READER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

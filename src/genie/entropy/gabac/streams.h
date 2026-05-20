/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines various custom stream buffer classes for handling data input
 * and output in the GENIE framework. These include buffers for file pointers,
 * in-memory data blocks, and special-purpose streams such as null streams. The
 * classes are designed to provide seamless integration with the standard C++
 * stream API.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_STREAMS_H_
#define SRC_GENIE_ENTROPY_GABAC_STREAMS_H_

// -----------------------------------------------------------------------------

#include <cstdio>
#include <istream>
#include <ostream>

#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Custom stream buffer wrapping a standard C-style FILE* pointer.
 * @details This class allows a `FILE*` pointer to be used as a standard stream
 * buffer, enabling easy integration with standard C++ I/O streams. This buffer
 * can be used for both input and output operations.
 */
class [[maybe_unused]] FileBuffer : public std::streambuf {
 public:
  /**
   * @brief Constructs a new `FileBuffer` using a given `FILE*` pointer.
   * @param f A pointer to the file to be wrapped by this stream buffer.
   */
  [[maybe_unused]] explicit FileBuffer(FILE* f);

 protected:
  /**
   * @brief Writes a single character to the buffer.
   * @param c The character to write.
   * @return The character written, or EOF on error.
   */
  int overflow(int c) override;

  /**
   * @brief Writes a sequence of characters to the buffer.
   * @param s Pointer to the character array to be written.
   * @param n Number of characters to write.
   * @return Number of characters successfully written.
   */
  std::streamsize xsputn(const char* s, std::streamsize n) override;

  /**
   * @brief Flushes the output buffer to the file.
   * @return Zero on success, or a non-zero value on failure.
   */
  int sync() override;

  /**
   * @brief Reads a sequence of characters from the buffer.
   * @param s Pointer to the buffer where characters will be stored.
   * @param n Maximum number of characters to read.
   * @return Number of characters successfully read.
   */
  std::streamsize xsgetn(char* s, std::streamsize n) override;

  /**
   * @brief Reads a single character from the buffer without advancing the
   * read position.
   * @return The character read, or EOF on error.
   */
  int underflow() override;

 private:
  FILE* fileptr_;  //!< Pointer to the file being operated on.
};

/**
 * @brief A stream buffer that operates on `util::DataBlock` instances.
 * @details This class provides a mechanism to treat `util::DataBlock` as a
 * stream, enabling read and write operations similar to those provided by
 * standard C++ streams.
 */
class DataBlockBuffer : public std::streambuf {
 public:
  /**
   * @brief Constructs a new `DataBlockBuffer`.
   * @param d Pointer to the `DataBlock` instance to operate on.
   * @param pos_i Initial position within the data block for reading (default
   * is 0).
   */
  explicit DataBlockBuffer(util::DataBlock* d, size_t pos_i = 0);

  /**
   * @brief Returns the Size of the underlying data block.
   * @return The number of bytes in the data block.
   */
  [[nodiscard]] size_t Size() const;

 protected:
  /**
   * @brief Writes a single character to the buffer.
   * @param c The character to write.
   * @return The character written, or EOF on error.
   */
  int overflow(int c) override;

  /**
   * @brief Writes a sequence of characters to the buffer.
   * @param s Pointer to the character array to be written.
   * @param n Number of characters to write.
   * @return Number of characters successfully written.
   */
  std::streamsize xsputn(const char* s, std::streamsize n) override;

  /**
   * @brief Reads a sequence of characters from the buffer.
   * @param s Pointer to the buffer where characters will be stored.
   * @param n Maximum number of characters to read.
   * @return Number of characters successfully read.
   */
  std::streamsize xsgetn(char* s, std::streamsize n) override;

  /**
   * @brief Reads a single character from the buffer without advancing the
   * read position.
   * @return The character read, or EOF on error.
   */
  int underflow() override;

  /**
   * @brief Reads a single character from the buffer and advances the read
   * position.
   * @return The character read, or EOF on error.
   */
  int uflow() override;

  /**
   * @brief Flushes the internal buffer to an external data block.
   * @param blk Pointer to the external `DataBlock` to be filled with data.
   */
  virtual void FlushBlock(util::DataBlock* blk);

  /**
   * @brief Moves the read or write position within the buffer.
   * @param sp The new position.
   * @return The resulting position.
   */
  pos_type seekpos(pos_type sp, std::ios_base::openmode) override;

  /**
   * @brief Moves the read or write position relative to a specified offset.
   * @param off Offset relative to the specified direction.
   * @param dir Direction for seeking.
   * @param which Specifies whether seeking is for input or output.
   * @return The resulting position.
   */
  pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                   std::ios_base::openmode which) override;

 private:
  util::DataBlock block_;  //!< The underlying data block.
  size_t pos_;             //!< Current position within the data block.
};

/**
 * @brief Input stream that wraps around a `DataBlockBuffer`.
 */
class IBufferStream : public DataBlockBuffer, public std::istream {
 public:
  /**
   * @brief Constructs a new input stream for a given data block.
   * @param d Pointer to the data block.
   * @param pos_i Initial position within the data block.
   */
  explicit IBufferStream(util::DataBlock* d, size_t pos_i = 0);
};

/**
 * @brief Output stream that wraps around a `DataBlockBuffer`.
 */
class OBufferStream : public DataBlockBuffer, public std::ostream {
 public:
  /**
   * @brief Constructs a new output stream for a given data block.
   * @param d Pointer to the data block.
   */
  explicit OBufferStream(util::DataBlock* d);

  /**
   * @brief Flushes the internal buffer to an external data block.
   * @param blk Pointer to the external `DataBlock` to be filled with data.
   */
  virtual void Flush(util::DataBlock* blk);
};

/**
 * @brief Stream buffer that discards all data written to it.
 */
class NullBuffer : public std::streambuf {
 protected:
  /**
   * @brief Ignores the character written to the buffer.
   * @param c Character to discard.
   * @return Always returns the input character.
   */
  int overflow(int c) override { return c; }
};

/**
 * @brief Stream that discards all data written to it.
 */
class NullStream : public std::ostream {
 public:
  /**
   * @brief Constructs a new `NullStream` that discards all output.
   */
  NullStream();

 private:
  NullBuffer m_sb_;  //!< Internal buffer that discards data.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_STREAMS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
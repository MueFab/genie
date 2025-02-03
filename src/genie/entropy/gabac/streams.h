/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_STREAMS_H_
#define SRC_GENIE_ENTROPY_GABAC_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>
#include <ostream>
#include "genie/util/data_block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief FILE* wrapper
 */
class FileBuffer : public std::streambuf {
 public:
    /**
     * @brief Create new buffer
     * @param f FILE* to operate on
     */
    explicit FileBuffer(FILE *f);

 protected:
    /**
     * @brief
     * @param c
     * @return
     */
    int overflow(int c) override;

    /**
     * @brief
     * @param s
     * @param n
     * @return
     */
    std::streamsize xsputn(const char *s, std::streamsize n) override;

    /**
     * @brief
     * @return
     */
    int sync() override;

    /**
     * @brief
     * @param s
     * @param n
     * @return
     */
    std::streamsize xsgetn(char *s, std::streamsize n) override;

    /**
     * @brief
     * @return
     */
    int underflow() override;

 private:
    FILE *fileptr;  //!<
};

/**
 * @brief Buffer for data blocks
 */
class DataBlockBuffer : public std::streambuf {
 public:
    /**
     * @brief Create new buffer
     * @param d Data block to operate on
     * @param pos_i Position to start in the block (mainly for input)
     */
    explicit DataBlockBuffer(util::DataBlock *d, size_t pos_i = 0);

    /**
     * @brief
     * @return
     */
    size_t size() const;

 protected:
    /**
     * @brief
     * @param c
     * @return
     */
    int overflow(int c) override;

    /**
     * @brief
     * @param s
     * @param n
     * @return
     */
    std::streamsize xsputn(const char *s, std::streamsize n) override;

    /**
     * @brief
     * @param s
     * @param n
     * @return
     */
    std::streamsize xsgetn(char *s, std::streamsize n) override;

    /**
     * @brief
     * @return
     */
    int underflow() override;

    /**
     * @brief
     * @return
     */
    int uflow() override;

    /**
     * @brief
     * @param blk
     */
    virtual void flush_block(util::DataBlock *blk);

    /**
     * @brief
     * @param sp
     * @return
     */
    pos_type seekpos(pos_type sp, std::ios_base::openmode) override;

    /**
     * @brief
     * @param off
     * @param dir
     * @param which
     * @return
     */
    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode which = std::ios_base::in) override;

 private:
    util::DataBlock block;  //!< @brief
    size_t pos;             //!< @brief
};

/**
 * @brief Buffer input
 */
class IBufferStream : public DataBlockBuffer, public std::istream {
 public:
    /**
     * @brief Create new stream.
     * @param d Data block to operate on.
     * @param pos_i Input starting position.
     */
    explicit IBufferStream(util::DataBlock *d, size_t pos_i = 0);
};

/**
 * @brief Buffer output
 */
class OBufferStream : public DataBlockBuffer, public std::ostream {
 public:
    /**
     * @brief Create new buffer
     * @param d data block to operate on
     * @todo check if parameter necessary
     */
    explicit OBufferStream(util::DataBlock *d);

    /**
     * @brief Swap internal block with external one
     * @param blk Block to fill with data
     */
    virtual void flush(util::DataBlock *blk);
};

/**
 * @brief Buffer doing nothing
 */
class NullBuffer : public std::streambuf {
 protected:
    /**
     * @brief
     * @param c
     * @return
     */
    int overflow(int c) override { return c; }
};

/**
 * @brief Stream doing nothing
 */
class NullStream : public std::ostream {
 public:
    /**
     * @brief Create new nullstream
     */
    NullStream();

 private:
    NullBuffer m_sb;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

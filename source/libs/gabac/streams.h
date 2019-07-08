/**
 * @file
 * @brief Gabac I/O streams
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_STREAMS_H_
#define GABAC_STREAMS_H_

#include <istream>
#include <ostream>

#include "data_block.h"

namespace gabac {

/**
 * @brief FILE* wrapper
 */
class FileBuffer : public std::streambuf
{
 public:
    /**
     * @brief Create new buffer
     * @param f FILE* to operate on
     */
    explicit FileBuffer(FILE *f);
 protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char *s, std::streamsize n) override;
    int sync() override;

    std::streamsize xsgetn(char *s, std::streamsize n) override;
    int underflow() override;
 private:
    FILE *fileptr;
};


/**
 * @brief Buffer for data blocks
 */
class DataBlockBuffer : public std::streambuf
{
 public:
    /**
     * @brief Create new buffer
     * @param d Data block to operate on
     * @param pos_i Position to start in the block (mainly for input)
     */
    explicit DataBlockBuffer(DataBlock *d, size_t pos_i = 0);
 protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char *s, std::streamsize n) override;
    std::streamsize xsgetn(char *s, std::streamsize n) override;
    int underflow() override;
    int uflow() override;
    virtual void flush_block(gabac::DataBlock *blk);
 private:
    DataBlock block;
    size_t pos;
};

/**
 * @brief File input
 */
class IFileStream : public FileBuffer, public std::istream
{
 public:
    /**
     * @brief Create new stream
     * @param f FILE* to operate on
     */
    explicit IFileStream(FILE *f);
};

/**
 * @brief File output
 */
class OFileStream : public FileBuffer, public std::ostream
{
 public:
    /**
     * @brief Create new stream
     * @param f FILE* to operate on
     */
    explicit OFileStream(FILE *f);
};

/**
 * @brief Buffer input
 */
class IBufferStream : public DataBlockBuffer, public std::istream
{
 public:
    /**
     * @brief Create new stream.
     * @param d Data block to operate on.
     * @param pos_i Input starting position.
     */
    explicit IBufferStream(DataBlock *d, size_t pos_i = 0);
};

/**
 * @brief Buffer output
 */
class OBufferStream : public DataBlockBuffer, public std::ostream
{
 public:
    /**
     * @brief Create new buffer
     * @param d data block to operate on
     * @todo check if parameter necessary
     */
    explicit OBufferStream(DataBlock *d);

    /**
     * @brief Swap internal block with external one
     * @param blk Block to fill with data
     */
    virtual void flush(gabac::DataBlock *blk){
        flush_block(blk);
    }
};

/**
 * @brief Buffer doing nothing
 */
class NullBuffer : public std::streambuf
{
 protected:
    int overflow(int c) override{
        return c;
    }
};

/**
 * @brief Stream doing nothing
 */
class NullStream : public std::ostream
{
 public:
    /**
     * @brief Create new nullstream
     */
    NullStream();

 private:
    NullBuffer m_sb;
};

}  // namespace gabac

#endif  // GABAC_STREAMS_H_

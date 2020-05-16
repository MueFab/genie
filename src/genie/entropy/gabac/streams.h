/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_STREAMS_H_
#define GABAC_STREAMS_H_

#include <istream>
#include <ostream>

#include <genie/util/data-block.h>

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
class DataBlockBuffer : public std::streambuf {
   public:
    /**
     * @brief Create new buffer
     * @param d Data block to operate on
     * @param pos_i Position to start in the block (mainly for input)
     */
    explicit DataBlockBuffer(util::DataBlock *d, size_t pos_i = 0);

    size_t size() const;

   protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char *s, std::streamsize n) override;
    std::streamsize xsgetn(char *s, std::streamsize n) override;
    int underflow() override;
    int uflow() override;
    virtual void flush_block(util::DataBlock *blk);

    pos_type seekpos(pos_type sp, std::ios_base::openmode) override { return pos = sp; }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode which = std::ios_base::in) override {
        (void)which;
        if (dir == std::ios_base::cur)
            pos = (off < 0 && size_t(std::abs(off)) > pos) ? 0 : std::min(pos + off, block.size());
        else if (dir == std::ios_base::end)
            pos = (off < 0 && size_t(std::abs(off)) > block.size()) ? 0 : std::min(block.size() + off, block.size());
        else if (dir == std::ios_base::beg)
            pos = (off < 0 && std::abs(off) > 0) ? 0 : std::min(size_t(0) + off, block.size());
        return pos;
    }

   private:
    util::DataBlock block;
    size_t pos;
};

/**
 * @brief File input
 */
class IFileStream : public FileBuffer, public std::istream {
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
class OFileStream : public FileBuffer, public std::ostream {
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
    virtual void flush(util::DataBlock *blk) { flush_block(blk); }
};

/**
 * @brief Buffer doing nothing
 */
class NullBuffer : public std::streambuf {
   protected:
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
    NullBuffer m_sb;
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_STREAMS_H_

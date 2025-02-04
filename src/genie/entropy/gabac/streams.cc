/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/streams.h"
#include <algorithm>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

FileBuffer::FileBuffer(FILE *f) : fileptr(f) {}

// ---------------------------------------------------------------------------------------------------------------------

int FileBuffer::overflow(int c) { return fputc(c, fileptr); }

// ---------------------------------------------------------------------------------------------------------------------

std::streamsize FileBuffer::xsputn(const char *s, std::streamsize n) {
    return fwrite(s, 1, static_cast<size_t>(n), fileptr);
}

// ---------------------------------------------------------------------------------------------------------------------

int FileBuffer::sync() { return fflush(fileptr); }

// ---------------------------------------------------------------------------------------------------------------------

std::streamsize FileBuffer::xsgetn(char *s, std::streamsize n) { return fread(s, 1, static_cast<size_t>(n), fileptr); }

// ---------------------------------------------------------------------------------------------------------------------

int FileBuffer::underflow() { return fgetc(fileptr); }

// ---------------------------------------------------------------------------------------------------------------------

DataBlockBuffer::DataBlockBuffer(util::DataBlock *d, size_t pos_i) : block(0, 1), pos(pos_i) { block.Swap(d); }

// ---------------------------------------------------------------------------------------------------------------------

size_t DataBlockBuffer::size() const { return block.GetRawSize(); }

// ---------------------------------------------------------------------------------------------------------------------

int DataBlockBuffer::overflow(int c) {
    block.PushBack(static_cast<uint64_t>(c));
    return c;
}

// ---------------------------------------------------------------------------------------------------------------------

std::streamsize DataBlockBuffer::xsputn(const char *s, std::streamsize n) {
    if (block.ModByWordSize(n)) {
        UTILS_DIE("Invalid Data length");
    }
    size_t oldSize = block.Size();
    block.Resize(block.Size() + block.DivByWordSize(n));
    memcpy(static_cast<uint8_t *>(block.GetData()) + block.MulByWordSize(oldSize), s, static_cast<size_t>(n));
    return n;
}

// ---------------------------------------------------------------------------------------------------------------------

std::streamsize DataBlockBuffer::xsgetn(char *s, std::streamsize n) {
    if (block.ModByWordSize(n)) {
        UTILS_DIE("Invalid Data length");
    }
    size_t bytesRead = std::min<size_t>(block.GetRawSize() - block.MulByWordSize(pos), size_t(n));
    memcpy(s, static_cast<uint8_t *>(block.GetData()) + block.MulByWordSize(pos), bytesRead);
    pos += block.DivByWordSize(bytesRead);
    return bytesRead;
}

// ---------------------------------------------------------------------------------------------------------------------

int DataBlockBuffer::underflow() {
    if (pos == block.Size()) {
        return EOF;
    }
    return static_cast<int>(block.Get(pos));
}

// ---------------------------------------------------------------------------------------------------------------------

int DataBlockBuffer::uflow() {
    if (pos == block.Size()) {
        return EOF;
    }
    return static_cast<int>(block.Get(pos++));
}

// ---------------------------------------------------------------------------------------------------------------------

void DataBlockBuffer::flush_block(util::DataBlock *blk) { block.Swap(blk); }

// ---------------------------------------------------------------------------------------------------------------------

std::streambuf::pos_type DataBlockBuffer::seekpos(pos_type sp, std::ios_base::openmode) { return pos = sp; }

// ---------------------------------------------------------------------------------------------------------------------

std::streambuf::pos_type DataBlockBuffer::seekoff(off_type off, std::ios_base::seekdir dir,
                                                  std::ios_base::openmode which) {
    (void)which;
    if (dir == std::ios_base::cur)
        pos = (off < 0 && size_t(std::abs(off)) > pos) ? 0 : std::min<size_t>(pos + off, block.Size());
    else if (dir == std::ios_base::end)
        pos =
            (off < 0 && size_t(std::abs(off)) > block.Size()) ? 0 : std::min<size_t>(block.Size() + off, block.Size());
    else if (dir == std::ios_base::beg)
        pos = (off < 0 && std::abs(off) > 0) ? 0 : std::min<size_t>(size_t(0) + off, block.Size());
    return pos;
}

// ---------------------------------------------------------------------------------------------------------------------

IBufferStream::IBufferStream(util::DataBlock *d, size_t pos_i) : DataBlockBuffer(d, pos_i), std::istream(this) {}

// ---------------------------------------------------------------------------------------------------------------------

OBufferStream::OBufferStream(util::DataBlock *d) : DataBlockBuffer(d, 0), std::ostream(this) {}

// ---------------------------------------------------------------------------------------------------------------------

void OBufferStream::flush(util::DataBlock *blk) { flush_block(blk); }

// ---------------------------------------------------------------------------------------------------------------------

NullStream::NullStream() : std::ostream(&m_sb) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

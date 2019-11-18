/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "streams.h"

#include <algorithm>

#include "exceptions.h"

namespace gabac {

FileBuffer::FileBuffer(FILE *f) : fileptr(f) {}

int FileBuffer::overflow(int c) { return fputc(c, fileptr); }

std::streamsize FileBuffer::xsputn(const char *s, std::streamsize n) {
    return fwrite(s, 1, static_cast<size_t>(n), fileptr);
}

int FileBuffer::sync() { return fflush(fileptr); }

std::streamsize FileBuffer::xsgetn(char *s, std::streamsize n) { return fread(s, 1, static_cast<size_t>(n), fileptr); }

int FileBuffer::underflow() { return fgetc(fileptr); }

DataBlockBuffer::DataBlockBuffer(DataBlock *d, size_t pos_i) : block(0, 1), pos(pos_i) { block.swap(d); }

size_t DataBlockBuffer::size() const { return block.getRawSize(); }

int DataBlockBuffer::overflow(int c) {
    block.push_back(static_cast<uint64_t>(c));
    return c;
}

std::streamsize DataBlockBuffer::xsputn(const char *s, std::streamsize n) {
    if (n % block.getWordSize()) {
        GABAC_DIE("Invalid Data length");
    }
    size_t oldSize = block.size();
    block.resize(block.size() + n / block.getWordSize());
    memcpy(static_cast<uint8_t *>(block.getData()) + oldSize * block.getWordSize(), s, static_cast<size_t>(n));
    return n;
}

std::streamsize DataBlockBuffer::xsgetn(char *s, std::streamsize n) {
    if (n % block.getWordSize()) {
        GABAC_DIE("Invalid Data length");
    }
    size_t bytesRead = std::min(block.getRawSize() - pos * block.getWordSize(), size_t(n));
    memcpy(s, static_cast<uint8_t *>(block.getData()) + pos * block.getWordSize(), bytesRead);
    pos += bytesRead / block.getWordSize();
    return bytesRead;
}

int DataBlockBuffer::underflow() {
    if (pos == block.size()) {
        return EOF;
    }
    return static_cast<int>(block.get(pos));
}

int DataBlockBuffer::uflow() {
    if (pos == block.size()) {
        return EOF;
    }
    return static_cast<int>(block.get(pos++));
}

void DataBlockBuffer::flush_block(gabac::DataBlock *blk) { block.swap(blk); }

IFileStream::IFileStream(FILE *f) : FileBuffer(f), std::istream(this) {}

OFileStream::OFileStream(FILE *f) : FileBuffer(f), std::ostream(this) {}

IBufferStream::IBufferStream(DataBlock *d, size_t pos_i) : DataBlockBuffer(d, pos_i), std::istream(this) {}

OBufferStream::OBufferStream(DataBlock *d) : DataBlockBuffer(d, 0), std::ostream(this) {}

NullStream::NullStream() : std::ostream(&m_sb) {}

}  // namespace gabac

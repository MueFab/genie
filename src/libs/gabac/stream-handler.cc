/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "stream-handler.h"
#include "data-block.h"

namespace gabac {

size_t StreamHandler::readStream(std::istream &input, DataBlock *buffer) {
    uint32_t streamSize = 0;
    input.read(reinterpret_cast<char *>(&streamSize), sizeof(uint32_t));
    return readBytes(input, streamSize, buffer);
}

size_t StreamHandler::readBytes(std::istream &input, size_t bytes, DataBlock *buffer) {
    if (bytes > 0) {
        if (bytes % buffer->getWordSize()) {
            GABAC_DIE("Input stream length not a multiple of word size");
        }
        buffer->resize(bytes / buffer->getWordSize());
        input.read(static_cast<char *>(buffer->getData()), bytes);
    }
    return bytes;
}

size_t StreamHandler::readFull(std::istream &input, DataBlock *buffer) {
    auto safe = input.exceptions();
    input.exceptions(std::ios::badbit);

    const size_t BUFFER_SIZE = size_t(1000000) / buffer->getWordSize();
    buffer->resize(0);
    while (input.good()) {
        size_t pos = buffer->size();
        buffer->resize(pos + BUFFER_SIZE);
        input.read(static_cast<char *>(buffer->getData()) + pos * buffer->getWordSize(),
                   BUFFER_SIZE * buffer->getWordSize());
    }
    if (!input.eof()) {
        GABAC_DIE("Error while reading input stream");
    }
    if (input.gcount() % buffer->getWordSize()) {
        GABAC_DIE("Input stream length not a multiple of word size");
    }
    buffer->resize(buffer->size() - (BUFFER_SIZE - input.gcount() / buffer->getWordSize()));
    input.exceptions(safe);
    return buffer->size();
}

size_t StreamHandler::readBlock(std::istream &input, size_t bytes, DataBlock *buffer) {
    auto safe = input.exceptions();
    input.exceptions(std::ios::badbit);

    if (bytes % buffer->getWordSize()) {
        GABAC_DIE("Input stream length not a multiple of word size");
    }
    const size_t BUFFER_SIZE = bytes / buffer->getWordSize();
    buffer->resize(BUFFER_SIZE);
    input.read(static_cast<char *>(buffer->getData()), BUFFER_SIZE * buffer->getWordSize());
    if (!input.good()) {
        if (!input.eof()) {
            GABAC_DIE("Error while reading input stream");
        }
        buffer->resize(buffer->size() - (BUFFER_SIZE - input.gcount() / buffer->getWordSize()));
    }
    input.exceptions(safe);
    return buffer->size();
}

size_t StreamHandler::writeStream(std::ostream &output, DataBlock *buffer) {
    uint32_t size = buffer->getRawSize();
    output.write(reinterpret_cast<char *>(&size), sizeof(uint32_t));
    return writeBytes(output, buffer);
}

size_t StreamHandler::writeBytes(std::ostream &output, DataBlock *buffer) {
    size_t ret = buffer->getRawSize();
    if (ret > 0) {
        output.write(static_cast<char *>(buffer->getData()), ret);
        buffer->clear();
    }
    return ret;
}

}  // namespace gabac

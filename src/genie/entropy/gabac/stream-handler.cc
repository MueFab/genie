/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "stream-handler.h"
#include <genie/util/data-block.h>
#include "exceptions.h"
namespace genie {
namespace entropy {
namespace gabac {

#define UINT_MAX_LENGTH (sizeof(uint64_t))
#define U7_MAX_LENGTH   (((sizeof(uint64_t) + 6) / 7) * 8)

uint64_t StreamHandler::readUInt(std::istream &input, size_t numBytes) {
    uint8_t bytes[UINT_MAX_LENGTH] = {0};

    input.read(reinterpret_cast<char *>(bytes), numBytes);

    uint64_t retVal = 0;
    size_t b = 0;
    while(numBytes-- > 0)
    {
        retVal = (retVal << 8) | bytes[b++];
    }
    return retVal;
}

size_t StreamHandler::readU7(std::istream &input) {
    uint64_t retVal = 0;
    size_t retValSize = 0;
    uint8_t byte;

    do {
        if (retValSize == U7_MAX_LENGTH) GABAC_DIE("readU7: out of range");
        retValSize++;

        input.read(reinterpret_cast<char *>(&byte), 1);
        retVal = (retVal << 7) | (byte & 0x7F);
    } while ((byte & 0x80) != 0);

    return retVal;
}

size_t StreamHandler::readStream(std::istream &input, util::DataBlock *buffer, size_t& numSymbols) {
    size_t streamSize = readUInt(input, 4);
    numSymbols = readUInt(input, 4);
    return readBytes(input, streamSize-4, buffer);
}

size_t StreamHandler::readBytes(std::istream &input, size_t bytes, util::DataBlock *buffer) {
    if (bytes > 0) {
        if (bytes % buffer->getWordSize()) {
            GABAC_DIE("Input stream length not a multiple of word size");
        }
        buffer->resize(bytes / buffer->getWordSize());
        input.read(static_cast<char *>(buffer->getData()), bytes);
    }
    return bytes;
}

size_t StreamHandler::readFull(std::istream &input, util::DataBlock *buffer) {
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

size_t StreamHandler::readBlock(std::istream &input, size_t bytes, util::DataBlock *buffer) {
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

void StreamHandler::writeUInt(std::ostream &output, uint64_t value, size_t numBytes) {
    uint8_t bytes[UINT_MAX_LENGTH] = {0};
    size_t b = 0;
    while(numBytes-- > 0) {
        bytes[b++] = (value >> (numBytes * 8)) & 0xFF;
    }
    output.write(reinterpret_cast<char *>(bytes), b);
}

void StreamHandler::writeU7(std::ostream &output, uint64_t value) {
    uint8_t bytes[U7_MAX_LENGTH] = {0};
    size_t b = 0;
    int shift;
    const int inputMaxSize = sizeof(value) * 8;
    for (shift = 0; (shift < inputMaxSize) && ((value >> shift) != 0);
         shift += 7);
    if (shift > 0) shift -= 7;

    for (; shift >= 0; shift -= 7) {
        uint8_t code = (uint8_t) (
                ((value >> shift) & 0x7F)
                |
                (shift > 0 ? 0x80 : 0x00));
        bytes[b++] = code;
    }

    output.write(reinterpret_cast<char *>(bytes), b);
}

size_t StreamHandler::writeStream(std::ostream &output, util::DataBlock *buffer, size_t numSymbols) {
    uint32_t streamSize = buffer->getRawSize() + 4;
    writeUInt(output, streamSize, 4);
    writeUInt(output, numSymbols, 4);
    return writeBytes(output, buffer);
}

size_t StreamHandler::writeBytes(std::ostream &output, util::DataBlock *buffer) {
    size_t ret = buffer->getRawSize();
    if (ret > 0) {
        output.write(static_cast<char *>(buffer->getData()), ret);
        buffer->clear();
    }
    return ret;
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie

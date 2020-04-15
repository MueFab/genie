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

size_t StreamHandler::readUInt(std::istream &input, uint64_t& retVal, size_t numBytes) {
    uint8_t bytes[UINT_MAX_LENGTH] = {0};

    input.read(reinterpret_cast<char *>(bytes), numBytes);

    retVal = 0;
    size_t c = 0; // counter
    while(numBytes-- > 0)
    {
        retVal = (retVal << 8) | bytes[c++];
    }

    return c;
}

size_t StreamHandler::readU7(std::istream &input, uint64_t& retVal) {
    size_t c = 0; // counter
    uint8_t byte;
    retVal = 0;

    do {
        if (c == U7_MAX_LENGTH) GABAC_DIE("readU7: out of range");
        c++;

        input.read(reinterpret_cast<char *>(&byte), 1);
        retVal = (retVal << 7) | (byte & 0x7F);
    } while ((byte & 0x80) != 0);

    return c;
}

size_t StreamHandler::readStream(std::istream &input, util::DataBlock *buffer, uint64_t& numSymbols) {
    size_t streamSize = 0;
    uint64_t payloadSize = 0 ;
    streamSize += readUInt(input, payloadSize, 4);

    numSymbols = 0;
    if(payloadSize >= 4) {
        streamSize += readUInt(input, numSymbols, 4);
        streamSize += readBytes(input, payloadSize-4, buffer);
    }

    return streamSize;
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

size_t StreamHandler::readStreamSize(std::istream& input) {
    input.seekg (0, input.end);
    size_t length = input.tellg();
    input.seekg (0, input.beg);

    return length;
}

size_t StreamHandler::writeUInt(std::ostream &output, uint64_t value, size_t numBytes) {
    uint8_t bytes[UINT_MAX_LENGTH] = {0};
    size_t c = 0; // counter
    while(numBytes-- > 0) {
        bytes[c++] = (value >> (numBytes * 8)) & 0xFF;
    }
    output.write(reinterpret_cast<char *>(bytes), c);

    return c;
}

size_t StreamHandler::writeU7(std::ostream &output, uint64_t value) {
    uint8_t bytes[U7_MAX_LENGTH] = {0};
    size_t c = 0; // counter
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
        bytes[c++] = code;
    }

    output.write(reinterpret_cast<char *>(bytes), c);

    return c;
}

size_t StreamHandler::writeStream(std::ostream &output, util::DataBlock *buffer, uint64_t numSymbols) {
    size_t streamSize = 0;
    uint64_t payloadSize = buffer->getRawSize()+((numSymbols)?4:0);
    streamSize += writeUInt(output, payloadSize, 4);
    if(payloadSize >= 4) {
        streamSize += writeUInt(output, numSymbols, 4);
        streamSize += writeBytes(output, buffer);
    }
    return streamSize;
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

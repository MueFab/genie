#include "gabac-subset/bit_input_stream.h"
#include "gabac-subset/block_stepper.h"
#include "gabac-subset/data_block.h"

#ifndef READ_H
#define READ_H

size_t track(uint64_t numBits) {
    static size_t bits_read = 0;
    bits_read += numBits;
    return bits_read;
}

int64_t readSigned(gabac::BitInputStream &instream, uint64_t numBits) {
    track(numBits);
    int64_t result = 0;
    while (numBits >= 8) {
        result = (result << 8) + instream.readByte();
        numBits -= 8;
    }
    result = (result << numBits) + instream.read(numBits);
    return result;
}

uint64_t readUnsigned(gabac::BitInputStream &instream, uint64_t numBits) {
    track(numBits);
    uint64_t result = 0;
    while (numBits >= 8) {
        result = (result << 8) + instream.readByte();
        numBits -= 8;
    }
    result = (result << numBits) + instream.read(numBits);
    return result;
}

uint64_t readU7(gabac::BitInputStream &instream) {
    uint64_t result = 0;
    unsigned char c;
    do {
        c = instream.readByte();
        track(8);
        result = (result << 7) | ((uint64_t)(c & 0x7f));
    } while (c & 0x80);
    return result;
}

std::string readString(gabac::BitInputStream &instream) {
    std::string result;
    while (true) {
        unsigned char c = instream.readByte();
        track(8);
        if (c == '\0') break;
        result += c;
    }
    return result;
}

#endif

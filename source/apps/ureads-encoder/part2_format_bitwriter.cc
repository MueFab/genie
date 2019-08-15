#include "part2_format_bitwriter.h"

void BitWriter::addToRest(uint64_t value, uint8_t bits) {
    uint64_t mask = ~uint64_t(0);
    rest <<= bits;
    rest &= (mask << bits);
    rest |= (value & (mask << (64u - bits))) >> (64u - bits);
    restSize = bits + restSize;
}

BitWriter::BitWriter(std::ostream &str) : stream(str), rest(0), restSize(0) {

}

void BitWriter::write(uint64_t value, uint8_t bits) {
    value <<= 64u - bits;
    if (bits + restSize < 8) {
        addToRest(value, bits);
        return;
    }
    uint8_t shiftbits = 8 - restSize;
    addToRest(value, shiftbits);
    value <<= shiftbits;
    stream.write(reinterpret_cast<char *>(&rest), 1);
    restSize = 0;
    rest = 0;
    bits -= shiftbits;

    while (bits >= 8) {
        stream.write(reinterpret_cast<char *>(&value) + 7, 1);
        bits -= 8;
        value <<= 8u;
    }

    addToRest(value, bits);
}

void BitWriter::flush() {

}

uint64_t BitWriter::getBitsWritten() {
    return bitsWritten;
}
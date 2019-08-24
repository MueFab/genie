#ifndef GENIE_PART2_FORMAT_BITWRITER_H
#define GENIE_PART2_FORMAT_BITWRITER_H

#include <ostream>
#include <cstdint>

class BitWriter {
private:
    std::ostream& stream;
    uint8_t rest;
    uint8_t restSize;
    uint64_t bitsWritten;
    void addToRest(uint64_t value, uint8_t bits);
public:
    BitWriter(std::ostream& str);
    void write (uint64_t value, uint8_t bits);
    void flush ();
    uint64_t getBitsWritten();

};


#endif //GENIE_PART2_FORMAT_BITWRITER_H

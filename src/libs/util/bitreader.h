#ifndef GENIE_BITREADER_H
#define GENIE_BITREADER_H

#include <cstdint>
#include <istream>

namespace util {
class BitReader {
   private:
    std::istream *m_istream;
    uint8_t m_currentPositionInChar;
    unsigned char m_currentChar;

    bool errorCheck();

   public:
    explicit BitReader(std::istream *istream);

    bool readBit(uint8_t *value);
    bool readNBits(uint32_t n, char *value);
};
}  // namespace util

#endif  // GENIE_BITREADER_H

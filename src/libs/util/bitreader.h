#ifndef GENIE_BITREADER_H
#define GENIE_BITREADER_H

#include <cstdint>
#include <istream>
#include <string>

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
    bool readNBits(uint32_t n, uint32_t *value);
    bool readNBitsDec(uint32_t n, u_int32_t *value);

    bool skipNBits(uint32_t bitsToSkip);

    bool isGood();

    void setFailBit() { m_istream->setstate(std::ios::failbit); }
};
}  // namespace util

#endif  // GENIE_BITREADER_H

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BITREADER_H
#define GENIE_BITREADER_H

#include <cstdint>
#include <istream>
#include <string>
#include <type_traits>

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

    uint64_t read(size_t) { return 0; }
    void read(std::istream *) {}

    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T read() {
        return sizeof(T) * 8;
    }

    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T read(size_t s) {
        return sizeof(T) * 8 * s;
    }

    void read(std::string &str) {
        for (auto &c : str) {
            c = char(read(8));
        }
    }

    bool isGood();

    void setFailBit() { m_istream->setstate(std::ios::failbit); }
};
}  // namespace util

#endif  // GENIE_BITREADER_H

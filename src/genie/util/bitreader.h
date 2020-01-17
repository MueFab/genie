/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BITREADER_H
#define GENIE_BITREADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <istream>
#include <string>
#include <type_traits>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

class BitReader {
   private:
    std::istream &istream;
    uint8_t m_heldBits;
    uint8_t m_numHeldBits;

   public:
    explicit BitReader(std::istream &_istream);

    uint64_t getByte();

    uint64_t flush();

    uint64_t read(uint8_t numBits);
    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T read() {
        return static_cast<T>(read(sizeof(T) * 8));
    }

    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T read(size_t s) {
        return static_cast<T>(read(s));
    }

    void read(std::string &str);

    bool isGood() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BITREADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
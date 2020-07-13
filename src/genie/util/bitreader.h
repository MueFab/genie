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

/**
 *
 */
class BitReader {
   private:
    std::istream &istream;  //!<
    uint8_t m_heldBits;     //!<
    uint8_t m_numHeldBits;  //!<

   public:
    /**
     *
     * @param _istream
     */
    explicit BitReader(std::istream &_istream);

    /**
     *
     * @return
     */
    uint64_t getByte();

    /**
     *
     * @return
     */
    uint64_t flush();

    /**
     *
     * @param numBits
     * @return
     */
    uint64_t read(uint8_t numBits);

    /**
     *
     * @tparam T
     * @return
     */
    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T read() {
        return static_cast<T>(read(sizeof(T) * 8));
    }

    /**
     *
     * @tparam T
     * @param s
     * @return
     */
    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T read(size_t s) {
        return static_cast<T>(read(s));
    }

    /**
     *
     * @param str
     */
    void read(std::string &str);

    /**
     *
     * @return
     */
    bool isGood() const;

    void readBuffer(void *in, size_t size) {
        istream.read((char*)in, size);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BITREADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
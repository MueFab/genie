/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BITREADER_H
#define GENIE_BITREADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>

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
    uint64_t bitsRead; //!<

   public:
    /**
     *
     * @return
     */
    uint64_t getBitsRead() const;

    /**
     *
     * @return
     */
    bool isAligned() const;

    /**
     *
     * @return
     */
    size_t getPos() const;

    /**
     *
     * @param pos
     */
    void setPos(size_t pos) const;

    /**
     *
     */
    void clear();

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
    T read();

    /**
     *
     * @tparam T
     * @param s
     * @return
     */
    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T read(size_t s);

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

    /**
     *
     * @param bytes
     */
    void skip(size_t bytes);

    /**
     *
     * @param in
     * @param size
     */
    void readBuffer(void *in, size_t size);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "bitreader.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BITREADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
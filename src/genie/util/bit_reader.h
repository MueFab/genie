/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BITREADER_H_
#define SRC_GENIE_UTIL_BITREADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>
#include <string>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Wrapper around an input stream to read data bit by bit instead of byte aligned.
 */
class BitReader {
 private:
    std::istream &istream;  //!< @brief Data input stream to fetch new bytes.
    uint8_t m_heldBits;     //!< @brief Bits from last byte which have not been consumed so far.
    uint8_t m_numHeldBits;  //!< @brief Number of bits from last byte which have not been consumed so far.
    uint64_t bitsRead;      //!< @brief Total number of bits read since the BitReader has been created.

 public:
    /**
     * @brief
     * @return Total number of bits read since the BitReader has been created.
     */
    uint64_t GetTotalBitsRead() const;

    /**
     * @brief
     * @return True if the position of the bitstream is aligned to a full byte.
     */
    bool IsByteAligned() const;

    /**
     * @brief
     * @return Returns the position in the input stream.
     */
    int64_t GetStreamPosition() const;

    /**
     * @brief Sets the input stream position.
     * @param pos New position.
     * @attention This bypasses the bit wise reading mechanism.
     */
    void SetStreamPosition(int64_t pos) const;

    /**
     * @brief Clears the error state of the underlying input stream.
     */
    void ClearStreamState();

    /**
     * @brief Create from existing input stream.
     * @param _istream Input stream to read.
     */
    explicit BitReader(std::istream &_istream);

    /**
     * @brief Get one byte of data from the input stream.
     * @attention This bypasses the bit wise reading. Only safe to use if position is aligned to a full byte.
     * @return One byte of data.
     */
    uint64_t ReadAlignedByte();

    /**
     * @brief Aligns the current position of the input stream to the next full byte.
     * @return 1 byte of data containing the remaining bits of the current byte.
     */
    uint64_t FlushHeldBits();

    /**
     * @brief Read a custom number of bits.
     * @param numBits Number of bits to read.
     * @return Integer containing the bits.
     */
    uint64_t ReadBits(uint8_t numBits);

    /**
     * @brief Reads a full integer with the required number of bits.
     * @tparam T Integer type. The number of bits to read is determined by this data type.
     * @return Integer read.
     */
    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T Read();

    /**
     * @brief Shortcut to the general read() function, but the return value is casted to a custom integer type.
     * @tparam T Return type you want to cast to.
     * @param s number of bits to read.
     * @return Custom integer.
     */
    template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
    T Read(uint8_t s);

    /**
     * @brief Read a string from the input.
     * @attention This bypasses the bit wise reading mechanism.
     * @param str The string to fill with data. Has to have the correct size already.
     */
    void readBypass(std::string &str);

    /**
     * @brief Read a string from the input.
     * @attention This bypasses the bit wise reading mechanism.
     * @param str The string to fill with data.
     */
    void ReadAlignedStringTerminated(std::string &str);

    /**
     * @brief
     * @return True if there are no problems so far reading data.
     */
    bool IsStreamGood() const;

    /**
     * @brief Skips a number of bytes
     * @attention This bypasses the bit wise reading mechanism.
     * @param bytes
     */
    void SkipAlignedBytes(size_t bytes);

    /**
     * @brief read
     * @param in
     * @param size
     * @attention This bypasses the bit wise reading mechanism.
     */
    void ReadAlignedBytes(void *in, size_t size);

    /**
     * @brief Read a single value as big endian
     * @tparam T Type name
     * @tparam SIZE Size of value
     * @return Value
     */
    template <typename T, size_t SIZE = sizeof(T), typename = std::enable_if<std::is_integral<T>::value>>
    T ReadAlignedInt();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit_reader.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BITREADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

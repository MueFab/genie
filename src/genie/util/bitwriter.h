/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BITWRITER_H
#define GENIE_BITWRITER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>
#include <ostream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Controlled output to an std::ostream. Allows to write single bits.
 */
class BitWriter {
   private:
    std::ostream *stream;    //!< @brief Where to direct output. A file for example.
    uint64_t m_heldBits;     //!< @brief Contains bits which cannot be written yet, as no byte is full.
    uint8_t m_numHeldBits;   //!< @brief How many bits there are in m_heldBits.
    uint64_t m_bitsWritten;  //!< @brief Counts number of written bits for statistical usages.

    /**
     * @brief Redirect assembled byte to the output stream
     * @param byte Output data
     */
    void writeOut(uint8_t byte);

   public:
    /**
     * @brief Create a bitwriter from an existing output stream.
     * @param str Some output stream. Must be valid to write to.
     */
    explicit BitWriter(std::ostream *str);

    /**
     * @brief Controlled destruction. Flush is calles, but stream not closed.
     */
    ~BitWriter();

    /**
     * @brief Write a specified number of bits
     * @param value Data to write. The LSBs will be written.
     * @param bits How many bits to write, range 1 to 64
     */
    void write(uint64_t value, uint8_t bits);

    /**
     * @brief Write all characters of string to the stream.
     * @param string String to write.
     */
    void write(const std::string &string);

    /**
     * @brief Write the whole data from another stream. Basically appending the data to this stream.
     * @param in Data source
     */
    void write(std::istream *in);

    /**
     * @brief Writes all buffered bits to the output stream. If there is no full byte available, the missing bits for
     * one full byte are filled with zeros. If no bits are currently buffered, nothing is written (not even zeros).
     */
    void flush();

    /**
     * @brief Reveals the already written number of bits.
     * @return m_bitsWritten is returned.
     */
    uint64_t getBitsWritten();

    /**
     * @return True if the stream position is aligned to a full byte.
     */
    bool isAligned() const;

    /**
     * @brief Write a full input stream to this output stream.
     * @param in Data source.
     */
    void writeBuffer(std::istream *in);

    /**
     * @brief Write a complete buffer
     * @param in Buffer.
     * @param size Size of buffer.
     * @attention This bypasses the bit by bit writing method.
     */
    void writeBuffer(const void *in, size_t size);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BITWRITER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
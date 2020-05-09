/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BITWRITER_H
#define GENIE_BITWRITER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>
#include "exceptions.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Controlled output to an std::ostream. Allows to write single bits.
 */
class BitWriter {
   private:
    std::ostream *stream;    //!< @brief Where to direct output. A file for example
    uint64_t m_heldBits;     //!< @brief Contains bits which cannot be written yet, as no byte is full
    uint8_t m_numHeldBits;   //!< @brief How many bits there are in m_heldBits;
    uint64_t m_bitsWritten;  //!< @brief Counts number of written bits for statistical usages

    /**
     * @brief Redirect assembled byte to the output stream
     * @param byte Output data
     */
    void writeOut(uint8_t byte);

   public:
    /**
     * @brief Create a bitwriter from an existing output stream.
     * @param str Some output stream. Must be valid to write to,
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

    void write(const std::string &string);

    /**
     * @brief Write the whole data from an other stream. Basically appending the data to this stream.
     * @param in Data source
     */
    void write(std::istream *in);

    /**
     * @brief Writes all buffered bits to the output stream. If there is no full byte available, the missing bits for
     * one full byte are filled with zeros. If no bits are currently buffered, nothing is written (not even zeros)
     */
    void flush();

    /**
     * @brief Reveals the already written number of bits
     * @return m_bitsWritten is returned
     */
    uint64_t getBitsWritten();

    bool isAligned() const { return m_numHeldBits == 0; }

    void writeBuffer(std::istream *in) {
        if (!isAligned()) {
            UTILS_DIE("Writer not aligned when it should be");
        }
        const size_t BUFFERSIZE = 100;
        char byte[BUFFERSIZE];
        do {
            in->read(byte, BUFFERSIZE);
            stream->write(byte, in->gcount());
            this->m_bitsWritten += in->gcount() * 8;
        } while (in->gcount() == BUFFERSIZE);
    }

    void writeBuffer(const void *in, size_t size) {
        this->m_bitsWritten += size * 8;
        if (!isAligned()) {
            UTILS_DIE("Writer not aligned when it should be");
        }
        stream->write((char *)in, size);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BITWRITER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
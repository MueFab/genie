/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_WRITER_H_
#define SRC_GENIE_CORE_WRITER_H_

#include <string>
#include <utility>
#include <vector>
#include "genie/util/bitwriter.h"

namespace genie {
namespace core {

class Writer {
 private:
    std::ostream* logwriter;
    util::BitWriter binwriter;
    bool writingLog;

 public:
    Writer(std::ostream* writer, bool log = false) : logwriter(writer), binwriter{writer}, writingLog(log){};
    /**
     * @brief Write a specified number of bits
     * @param value Data to write. The LSBs will be written.
     * @param bits How many bits to write, range 1 to 64
     */
    void write(uint64_t value, uint8_t bits) {
        if (writingLog)
            *logwriter << std::to_string(value) << ",";
        else
            binwriter.write(value, bits);
    }

    /**
     * @brief Write all characters of string to the stream.
     * @param string String to write.
     */
    void write(const std::string& string) {
        if (writingLog)
            *logwriter << '"' << string << '"' << ",";
        else
            binwriter.write(string);
    }
    /**
     * @brief Write the whole data from another stream. Basically appending the data to this stream.
     * @param in Data source
     */
    void write(std::istream* in) {
        if (!writingLog) binwriter.write(in);
    };

    /**
     * @brief Writes all buffered bits to the output stream. If there is no full byte available, the missing bits for
     * one full byte are filled with zeros. If no bits are currently buffered, nothing is written (not even zeros).
     */
    void flush() {
        if (!writingLog) binwriter.flush();
    };

    /**
     * @brief Reveals the already written number of bits.
     * @return m_bitsWritten is returned.
     */
    uint64_t getBitsWritten() const {
        if (!writingLog)
            return binwriter.getBitsWritten();
        else
            return 0;
    }
};

}  // namespace core
}  // namespace genie
#endif  // SRC_GENIE_CORE_WRITER_H_
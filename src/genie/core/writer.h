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
    util::BitWriter binwriter{logwriter};
    bool writingLog;
    bool getWriteSize;
    size_t writeBitSize;

 public:
    Writer() : logwriter(nullptr), writingLog(false), getWriteSize(true), writeBitSize(0) {}

    Writer(std::ostream* writer, bool log = false)
        : logwriter(writer), binwriter{writer}, writingLog(log), getWriteSize(false), writeBitSize(0){};
    /**
     * @brief Write a specified number of bits, reserved are not written to log
     * @param value Data to write. The LSBs will be written.
     * @param bits How many bits to write, range 1 to 64
     */
    void write(uint64_t value, uint8_t bits, bool reserved) {
        if (getWriteSize) {
            writeBitSize += bits;
        } else if (reserved) {
            if (!writingLog) write(value, bits);
        } else {
            write(value, bits);
        }
    }
    void write_reserved(uint8_t bits) { write(0, bits, true); }
    /**
     * @brief Write a specified number of bits
     * @param value Data to write. The LSBs will be written.
     * @param bits How many bits to write, range 1 to 64
     */
    void write(uint64_t value, uint8_t bits) {
        if (getWriteSize) {
            writeBitSize += bits;
        } else if (writingLog)
            *logwriter << std::to_string(value) << ",";
        else
            binwriter.write(value, bits);
    }

    /**
     * @brief Write all characters of string to the stream.
     * @param string String to write.
     */
    void write(const std::string& str) {
        if (getWriteSize) {
            writeBitSize += str.size() * 8;
        } else if (writingLog) {
            if (str.empty())
                *logwriter << '"' << '"' << ",";
            else
                *logwriter << '"' << str << '"' << ",";
        } else {
            if (!str.empty()) binwriter.write(str);
        }
    }
    /**
     * @brief Write the whole data from another stream. Basically appending the data to this stream.
     * @param in Data source
     */
    void write(std::istream* in) {
        uint8_t byte;
        if (getWriteSize) {
            in->seekg(0, in->end);
            writeBitSize += in->tellg() * 8;
            in->seekg(0, in->beg);
        } else if (!writingLog)
            while (in->read((char*)&byte, sizeof(byte))) {
                binwriter.write(byte, 8);
            }
    };

    /**
     * @brief Writes all buffered bits to the output stream. If there is no full byte available, the missing bits for
     * one full byte are filled with zeros. If no bits are currently buffered, nothing is written (not even zeros).
     */
    void flush() {
        if (getWriteSize) {
            if (writeBitSize % 8 != 0) writeBitSize += (8 - writeBitSize % 8);
        } else if (!writingLog)
            binwriter.flush();
    };

    /**
     * @brief Reveals the already written number of bits.
     * @return m_bitsWritten is returned.
     */
    uint64_t getBitsWritten() const {
        if (getWriteSize) {
            return writeBitSize;
        } else if (!writingLog)
            return binwriter.getBitsWritten();
        else
            return 0;
    }
};

}  // namespace core
}  // namespace genie
#endif  // SRC_GENIE_CORE_WRITER_H_
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
#include "genie/util/bit_writer.h"

namespace genie {
namespace core {
class FilePos {
 public:
    typedef std::pair<uint64_t, uint8_t> Pos;

    static Pos add(const Pos a, const Pos b) {
        Pos added = {a.first + b.first, a.second + b.second};
        if (added.second > 8) {
            added.second -= 8;
            added.first++;
        }
        return added;
    }

    static Pos sub(Pos a, Pos b) {
        Pos substracted = {0, 0};
        if (a.first < b.first || (a.first == b.first && a.second <= b.second)) return substracted;
        substracted.first = a.first - b.first;
        if (a.second < b.second) {
            substracted.first--;
            substracted.second = static_cast<uint8_t>(8 + a.second - b.second);
        } else {
            substracted.second = a.second - b.second;
        }
        return substracted;
    }
};

class Writer {
 private:
    std::ostream* logwriter;
    util::BitWriter binwriter{logwriter};
    bool writingLog;
    bool getWriteSize;
    size_t writeBitSize;
    const char endVal = '\n';

 public:
    Writer() : logwriter(nullptr), writingLog(false), getWriteSize(true), writeBitSize(0) {}

    explicit Writer(std::ostream* writer, bool log = false)
        : logwriter(writer), binwriter(writer), writingLog(log), getWriteSize(false), writeBitSize(0) {}

    bool IsLogWriter() const { return writingLog; }
    util::BitWriter& GetBinWriter() { return binwriter; }
    /**
     * @brief Write a specified number of bits, reserved are not written to log
     * @param value Data to write. The LSBs will be written.
     * @param bits How many bits to write, range 1 to 64
     */
    void Write(uint64_t value, uint8_t bits, bool reserved) {
        if (getWriteSize) {
            writeBitSize += bits;
        } else if (reserved) {
            if (!writingLog) Write(value, bits);
        } else {
          Write(value, bits);
        }
    }
    void WriteReserved(uint8_t bits) { Write(0, bits, true); }
    /**
     * @brief Write a specified number of bits
     * @param value Data to write. The LSBs will be written.
     * @param bits How many bits to write, range 1 to 64
     */
    void Write(uint64_t value, uint8_t bits) {
        if (getWriteSize) {
            writeBitSize += bits;
        } else if (writingLog) {
            *logwriter << std::to_string(value) << endVal;
        } else if (bits > 0) {
            binwriter.WriteBits(value, bits);
        }
    }

    /**
     * @brief Write all characters of string to the stream.
     * @param string String to write.
     */
    void Write(const std::string& str) {
        if (getWriteSize) {
            writeBitSize += str.size() * 8;
        } else if (writingLog) {
            if (str.empty())
                *logwriter << '"' << '"' << endVal;
            else
                *logwriter << '"' << str << '"' << endVal;
        } else {
            if (!str.empty()) binwriter.Write(str);
        }
    }
    /**
     * @brief Write the whole data from another stream. Basically appending the data to this stream.
     * @param in Data source
     */
    void Write(std::istream* in) {
        uint8_t byte;
        if (getWriteSize) {
            in->seekg(0, in->end);
            writeBitSize += in->tellg() * 8;
            in->seekg(0, in->beg);
        } else if (!writingLog) {
            while (in->read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
                binwriter.WriteBits(byte, 8);
            }
        }
    }

    /**
     * @brief Writes all buffered bits to the output stream. If there is no full byte available, the missing bits for
     * one full byte are filled with zeros. If no bits are currently buffered, nothing is written (not even zeros).
     */
    void Flush() {
        if (getWriteSize) {
            if (writeBitSize % 8 != 0) writeBitSize += (8 - writeBitSize % 8);
        } else if (!writingLog) {
            binwriter.FlushBits();
        }
    }

    /**
     * @brief Reveals the already written number of bits.
     * @return m_bitsWritten is returned.
     */
    uint64_t GetBitsWritten() const {
        if (getWriteSize) {
            return writeBitSize;
        } else if (!writingLog) {
            return binwriter.GetTotalBitsWritten();
        } else {
            return 0;
        }
    }

      /**
     * @brief Reveals stream object.
     */
    std::ostream* getStreamObj() { return logwriter; }

    /**
     * @brief Use for implicit conversion where BitWriter is expected. (Temp fix!)
     */
    operator util::BitWriter&() {
        return binwriter;
    }

};
}  // namespace core
}  // namespace genie
#endif  // SRC_GENIE_CORE_WRITER_H_

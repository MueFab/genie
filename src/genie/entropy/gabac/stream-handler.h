/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_STREAM_HANDLER_H_
#define GABAC_STREAM_HANDLER_H_

#include <istream>
#include <ostream>

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie
namespace genie {
namespace entropy {
namespace gabac {

class StreamHandler {
   public:
    static size_t readUInt(std::istream& input, uint64_t& retVal, size_t numBytes);
    static size_t readU7(std::istream& input, uint64_t& retVal);
    static size_t readStream(std::istream& input, util::DataBlock* buffer, uint64_t& numSymbols);
    static size_t readBytes(std::istream& input, size_t bytes, util::DataBlock* buffer);
    static size_t readFull(std::istream& input, util::DataBlock* buffer);
    static size_t readBlock(std::istream& input, size_t bytes, util::DataBlock* buffer);
    static size_t readStreamSize(std::istream& input);

    static size_t writeUInt(std::ostream& output, uint64_t value, size_t numBytes);
    static size_t writeU7(std::ostream& output, uint64_t value);
    static size_t writeStream(std::ostream& output, util::DataBlock* buffer, uint64_t numSymbols);
    static size_t writeBytes(std::ostream& output, util::DataBlock* buffer);
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_STREAM_HANDLER_H_

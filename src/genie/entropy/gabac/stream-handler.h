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
    static size_t readStream(std::istream& input, util::DataBlock* buffer);
    static size_t readBytes(std::istream& input, size_t bytes, util::DataBlock* buffer);
    static size_t readFull(std::istream& input, util::DataBlock* buffer);
    static size_t readBlock(std::istream& input, size_t bytes, util::DataBlock* buffer);
    static size_t writeStream(std::ostream& output, util::DataBlock* buffer);
    static size_t writeBytes(std::ostream& output, util::DataBlock* buffer);
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_STREAM_HANDLER_H_

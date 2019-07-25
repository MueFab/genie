/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_STREAM_HANDLER_H_
#define GABAC_STREAM_HANDLER_H_

#include <istream>
#include <ostream>

namespace gabac {

class DataBlock;

class StreamHandler
{
 public:
    static size_t readStream(std::istream& input, DataBlock *buffer);
    static size_t readBytes(std::istream& input, size_t bytes, DataBlock *buffer);
    static size_t readFull(std::istream& input, DataBlock *buffer);
    static size_t readBlock(std::istream& input, size_t bytes, DataBlock *buffer);
    static size_t writeStream(std::ostream& output, DataBlock *buffer);
    static size_t writeBytes(std::ostream& output, DataBlock *buffer);
};
}  // namespace gabac

#endif  // GABAC_STREAM_HANDLER_H_

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENIE_GABAC_OUTPUT_STREAM_H
#define GENIE_GENIE_GABAC_OUTPUT_STREAM_H

#include <genie/entropy/gabac/gabac.h>

namespace genie {
namespace entropy {
namespace gabac {

class GenieGabacOutputBuffer : public std::streambuf {
   public:
    GenieGabacOutputBuffer();

   protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char *s, std::streamsize n) override;

   private:
    std::vector<util::DataBlock> streams;
    std::streamsize bytesLeft;
    std::vector<uint8_t> sizeBuf;

   public:
    void flush_blocks(std::vector<util::DataBlock> *dat);
};

class GenieGabacOutputStream : public GenieGabacOutputBuffer, public std::ostream {
   public:
    GenieGabacOutputStream();
};

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GENIE_GENIE_GABAC_OUTPUT_STREAM_H

// Copyright 2018 The genie authors

#ifndef GENIE_GENIE_GABAC_OUTPUT_STREAM_H
#define GENIE_GENIE_GABAC_OUTPUT_STREAM_H

#include <gabac/gabac.h>

namespace lae {

class GenieGabacOutputBuffer : public std::streambuf {
   public:
    GenieGabacOutputBuffer();

   protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char *s, std::streamsize n) override;

   private:
    std::vector<gabac::DataBlock> streams;
    std::streamsize bytesLeft;
    std::vector<uint8_t> sizeBuf;

   public:
    void flush_blocks(std::vector<gabac::DataBlock> *dat);
};

class GenieGabacOutputStream : public GenieGabacOutputBuffer, public std::ostream {
   public:
    GenieGabacOutputStream() : GenieGabacOutputBuffer(), std::ostream(this) {}
};

}  // namespace genie

#endif  // GENIE_GENIE_GABAC_OUTPUT_STREAM_H

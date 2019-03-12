// Copyright 2018 The genie authors

#ifndef GENIE_GENIE_GABAC_OUTPUT_STREAM_H
#define GENIE_GENIE_GABAC_OUTPUT_STREAM_H

#include "gabac/gabac.h"

namespace dsg {

class GenieGabacOutputStream : public gabac::OutputStream
{
 private:
    std::vector <std::pair<size_t, uint8_t *>> streams;
    size_t bytes;
 public:

    GenieGabacOutputStream();
    size_t writeStream(gabac::DataBlock *inbuffer) override;
    size_t writeBytes(gabac::DataBlock *) override;
    size_t bytesWritten() override;
    void flush(std::vector <std::pair<size_t, uint8_t *>> *dat);
};
}

#endif //GENIE_GENIE_GABAC_OUTPUT_STREAM_H

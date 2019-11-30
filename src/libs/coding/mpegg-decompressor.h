#ifndef GENIE_MPEGG_DECOMPRESSOR_H
#define GENIE_MPEGG_DECOMPRESSOR_H

#include "mpegg-raw-au.h"

#include <memory>
#include <format/block-payload.h>

class MpeggDecompressor {
public:
    virtual std::unique_ptr<MpeggRawAu> decompress(std::unique_ptr<BlockPayload> payload) = 0;
    virtual ~MpeggDecompressor() = default;
};


#endif //GENIE_MPEGG_DECOMPRESSOR_H

#ifndef GENIE_MPEGG_COMPRESSOR_H
#define GENIE_MPEGG_COMPRESSOR_H

#include "mpegg-raw-au.h"

#include <format/block-payload.h>
#include <memory>

#include <format/block-payload.h>
#include <util/source.h>

class MpeggCompressor : public Source<BlockPayloadSet>, public Drain<MpeggRawAu> {
   public:
    void flowIn(MpeggRawAu&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggCompressor() override = default;
};

#endif  // GENIE_MPEGG_COMPRESSOR_H

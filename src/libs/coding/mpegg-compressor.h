#ifndef GENIE_MPEGG_COMPRESSOR_H
#define GENIE_MPEGG_COMPRESSOR_H

#include "mpegg-raw-au.h"

#include <format/block-payload.h>
#include <memory>

#include <format/block-payload.h>
#include <util/source.h>

class MpeggCompressor : public Source<std::unique_ptr<BlockPayloadSet>>, public Drain<std::unique_ptr<MpeggRawAu>> {
   public:
    void flowIn(std::unique_ptr<MpeggRawAu> t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggCompressor() override = default;
};

#endif  // GENIE_MPEGG_COMPRESSOR_H

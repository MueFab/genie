#ifndef GENIE_MPEGG_COMPRESSOR_H
#define GENIE_MPEGG_COMPRESSOR_H

#include "mpegg-raw-au.h"

#include <backbone/block-payload.h>
#include <memory>

#include <backbone/block-payload.h>
#include <util/source.h>

namespace genie {

class MpeggCompressor : public util::Source<BlockPayloadSet>, public util::Drain<MpeggRawAu> {
   public:
    void flowIn(MpeggRawAu&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggCompressor() override = default;
};

}  // namespace genie

#endif  // GENIE_MPEGG_COMPRESSOR_H

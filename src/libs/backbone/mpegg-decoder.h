#ifndef GENIE_MPEGG_DECODER_H
#define GENIE_MPEGG_DECODER_H

#include <backbone/block-payload.h>
#include <memory>
#include <vector>
#include "mpegg-raw-au.h"

#include <util/source.h>

namespace genie {

class MpeggDecoder : public util::Source<mpegg_rec::MpeggChunk>, public util::Drain<MpeggRawAu> {
   public:
    void flowIn(MpeggRawAu&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggDecoder() override = default;
};

}  // namespace genie

#endif  // GENIE_MPEGG_DECODER_H

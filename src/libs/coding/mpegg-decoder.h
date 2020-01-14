#ifndef GENIE_MPEGG_DECODER_H
#define GENIE_MPEGG_DECODER_H

#include <format/block-payload.h>
#include <memory>
#include <vector>
#include "mpegg-raw-au.h"

#include <util/source.h>

namespace coding {

class MpeggDecoder : public Source<format::mpegg_rec::MpeggChunk>, public Drain<MpeggRawAu> {
   public:
    void flowIn(MpeggRawAu&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggDecoder() override = default;
};

}  // namespace coding

#endif  // GENIE_MPEGG_DECODER_H

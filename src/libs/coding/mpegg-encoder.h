#ifndef GENIE_MPEGG_ENCODER_H
#define GENIE_MPEGG_ENCODER_H

#include <memory>
#include <vector>
#include "mpegg-raw-au.h"

#include <util/drain.h>
#include <util/source.h>

namespace coding {

class MpeggEncoder : public Drain<format::mpegg_rec::MpeggChunk>, public Source<MpeggRawAu> {
   public:
    void flowIn(format::mpegg_rec::MpeggChunk&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggEncoder() override = default;
};
}  // namespace coding

#endif  // GENIE_MPEGG_ENCODER_H

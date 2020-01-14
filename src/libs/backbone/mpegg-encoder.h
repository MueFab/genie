#ifndef GENIE_MPEGG_ENCODER_H
#define GENIE_MPEGG_ENCODER_H

#include <memory>
#include <vector>
#include "mpegg-raw-au.h"

#include <util/drain.h>
#include <util/source.h>

namespace genie {

class MpeggEncoder : public util::Drain<genie::mpegg_rec::MpeggChunk>, public util::Source<MpeggRawAu> {
   public:
    ~MpeggEncoder() override = default;
};
}  // namespace genie

#endif  // GENIE_MPEGG_ENCODER_H

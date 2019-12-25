#ifndef GENIE_MPEGG_ENCODER_H
#define GENIE_MPEGG_ENCODER_H

#include <memory>
#include <vector>
#include "mpegg-raw-au.h"

#include <util/drain.h>
#include <util/source.h>

class MpeggEncoder : public Drain<std::unique_ptr<format::mpegg_rec::MpeggChunk>>,
                     public Source<std::unique_ptr<MpeggRawAu>> {
   public:
    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggEncoder() override = default;
};

#endif  // GENIE_MPEGG_ENCODER_H

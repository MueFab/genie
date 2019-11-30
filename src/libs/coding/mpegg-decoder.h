#ifndef GENIE_MPEGG_DECODER_H
#define GENIE_MPEGG_DECODER_H

#include <memory>
#include <vector>
#include <format/block-payload.h>
#include "mpegg-raw-au.h"

#include <util/source.h>


class MpeggDecoder : public Source<std::unique_ptr<format::mpegg_rec::MpeggChunk>>, public Drain<std::unique_ptr<MpeggRawAu>>{
public:
    void flowIn(std::unique_ptr<MpeggRawAu> t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggDecoder() override = default;
};

#endif //GENIE_MPEGG_DECODER_H

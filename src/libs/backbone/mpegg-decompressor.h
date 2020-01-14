/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MPEGG_DECOMPRESSOR_H
#define GENIE_MPEGG_DECOMPRESSOR_H

#include "mpegg-raw-au.h"

#include <backbone/block-payload.h>
#include <util/source.h>
#include <memory>

namespace genie {

class MpeggDecompressor : public util::Drain<BlockPayloadSet>, public util::Source<MpeggRawAu> {
   public:
    void flowIn(BlockPayloadSet&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~MpeggDecompressor() override = default;
};

}  // namespace genie

#endif  // GENIE_MPEGG_DECOMPRESSOR_H

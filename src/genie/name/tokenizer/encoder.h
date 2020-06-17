/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENCODER_H
#define GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/watch.h>
#include <limits>
#include <string>
#include <vector>
#include "genie/core/name-encoder.h"
#include "genie/core/record/record.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

#include "tokenizer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

class Encoder : public core::NameEncoder {
   public:
    std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> process(const core::record::Chunk& recs) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
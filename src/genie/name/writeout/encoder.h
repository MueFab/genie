/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_NAME_WRITEOUT_ENCODER_H_
#define SRC_GENIE_NAME_WRITEOUT_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include <tuple>
#include <vector>
#include "genie/core/name-encoder.h"
#include "genie/core/record/record.h"
#include "genie/name/tokenizer/tokenizer.h"
#include "genie/util/data-block.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::name::writeout {

/**
 * @brief
 */
class Encoder : public core::NameEncoder {
 public:
    /**
     * @brief
     * @param recs
     * @return
     */
    std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> process(const core::record::Chunk& recs) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::name::writeout

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_WRITEOUT_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

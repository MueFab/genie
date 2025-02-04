/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_NAME_TOKENIZER_ENCODER_H_
#define SRC_GENIE_NAME_TOKENIZER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include <tuple>
#include <vector>
#include "genie/core/name_encoder.h"
#include "genie/core/record/alignment/record.h"
#include "genie/name/tokenizer/tokenizer.h"
#include "genie/util/data_block.h"
#include "genie/util/stop_watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

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

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_TOKENIZER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENCODER_H
#define GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/record/record.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

#include "tokenizer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::vector<genie::util::DataBlock>> generate_id_tokens(const genie::core::record::Chunk& recs) {
    std::vector<std::vector<genie::util::DataBlock>> ret;
    std::vector<SingleToken> old;

    for (const auto& r : recs) {
        TokenState state(old, r.getName());
        auto newTok = state.run();
        TokenState::encode(newTok, ret);
        old = patch(old, newTok);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "spring-encoder.h"

namespace genie {
namespace read {
namespace spring {

void SpringEncoder::flowIn(core::record::Chunk &&t, size_t id) {
    preprocessor.preprocess(std::move(t), id);
}

void SpringEncoder::dryIn() {
    preprocessor.finish();



    dryOut();
}
}  // namespace spring
}  // namespace read_coding
}  // namespace genie
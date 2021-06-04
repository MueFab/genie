/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/classifier-bypass.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

record::Chunk ClassifierBypass::getChunk() {
    flushing = false;
    record::Chunk ret;
    if (vec.empty()) {
        return ret;
    }
    ret = std::move(vec.front());
    vec.erase(vec.begin());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierBypass::add(record::Chunk&& c) { vec.emplace_back(std::move(c)); }

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierBypass::flush() { flushing = true; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

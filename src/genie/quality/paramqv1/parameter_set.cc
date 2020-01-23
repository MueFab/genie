/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "parameter_set.h"
#include <genie/util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::addCodeBook(Codebook &&book) {
    qv_codebooks.emplace_back(std::move(book));
}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::write(util::BitWriter &writer) const {
    writer.write(qv_codebooks.size(), 4);
    for(const auto& q : qv_codebooks) {
        q.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
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

bool ParameterSet::operator==(const ParameterSet &ps) const { return qv_codebooks == ps.qv_codebooks; }

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::addCodeBook(Codebook &&book) { qv_codebooks.emplace_back(std::move(book)); }

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::write(util::BitWriter &writer) const {
    writer.write(qv_codebooks.size(), 4);
    for (const auto &q : qv_codebooks) {
        q.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet::ParameterSet(util::BitReader &reader) {
    const size_t num_codebooks = reader.read<int>(4);
    for (auto c = num_codebooks; c > 0; --c) {
        qv_codebooks.emplace_back(Codebook(reader));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Codebook> &ParameterSet::getCodebooks() const { return qv_codebooks; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
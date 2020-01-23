/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "codebook.h"
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

Codebook::Codebook(util::BitReader &reader) {
    qv_recon.resize(reader.read<uint8_t >());
    for(auto& v : qv_recon) {
        v = reader.read<uint8_t >();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Codebook::addEntry(uint8_t entry) {
    constexpr size_t BOOK_MAX_SIZE = 94;
    UTILS_DIE_IF(qv_recon.size() == BOOK_MAX_SIZE, "Maximum codebook size exceeded");
    qv_recon.emplace_back(entry);
}

// ---------------------------------------------------------------------------------------------------------------------

void Codebook::write(util::BitWriter &writer) const {
    writer.write(qv_recon.size(), 8);
    for(const auto& v : qv_recon) {
        writer.write(v, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
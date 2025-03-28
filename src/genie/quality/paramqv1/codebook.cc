/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/paramqv1/codebook.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::paramqv1 {

// ---------------------------------------------------------------------------------------------------------------------

bool Codebook::operator==(const Codebook& ps) const { return qv_recon == ps.qv_recon; }

// ---------------------------------------------------------------------------------------------------------------------

Codebook::Codebook(util::BitReader& reader) {
    qv_recon.resize(reader.Read<uint8_t>());
    for (auto& v : qv_recon) {
        v = reader.Read<uint8_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Codebook::addEntry(uint8_t entry) {
    constexpr size_t BOOK_MAX_SIZE = 94;
    UTILS_DIE_IF(qv_recon.size() == BOOK_MAX_SIZE, "Maximum codebook size exceeded");
    qv_recon.emplace_back(entry);
}

// ---------------------------------------------------------------------------------------------------------------------

void Codebook::write(util::BitWriter& writer) const {
    writer.write(qv_recon.size(), 8);
    for (const auto& v : qv_recon) {
        writer.write(v, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Codebook::Codebook(uint8_t v1, uint8_t v2) {
    qv_recon.push_back(v1);
    qv_recon.push_back(v2);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint8_t>& Codebook::getEntries() const { return qv_recon; }

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genie::quality::paramqv1



// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

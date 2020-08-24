/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "segment.h"
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/runtime-exception.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

Segment::Segment() : sequence(), quality_values() {}

// ---------------------------------------------------------------------------------------------------------------------

Segment::Segment(std::string&& _sequence) : sequence(std::move(_sequence)), quality_values() {}

// ---------------------------------------------------------------------------------------------------------------------

Segment::Segment(uint32_t length, uint8_t qv_depth, util::BitReader& reader) {
    this->sequence = std::string(length, 0);
    reader.readBypass(this->sequence);
    this->quality_values.resize(qv_depth);
    for (auto& q : quality_values) {
        q.resize(length, 0);
        reader.readBypass(q);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Segment::getSequence() const { return sequence; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Segment::getQualities() const { return quality_values; }

// ---------------------------------------------------------------------------------------------------------------------

void Segment::addQualities(std::string&& qv) {
    UTILS_DIE_IF(qv.length() != sequence.length(), "QV and sequence lengths do not match");
    quality_values.emplace_back(std::move(qv));
}

// ---------------------------------------------------------------------------------------------------------------------

void Segment::write(util::BitWriter& writer) const {
    writer.write(this->sequence);
    for (const auto& a : this->quality_values) {
        writer.write(a);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
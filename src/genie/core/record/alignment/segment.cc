/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "segment.h"
#include <utility>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

Segment::Segment() : sequence(), quality_values() {}

// ---------------------------------------------------------------------------------------------------------------------

Segment::Segment(std::string&& _sequence) : sequence(std::move(_sequence)), quality_values() {}

// ---------------------------------------------------------------------------------------------------------------------

Segment::Segment(uint32_t length, uint8_t qv_depth, util::BitReader& reader)
    : sequence(length, 0), quality_values(qv_depth, std::string(length, 0)) {
    reader.ReadAlignedBytes(&this->sequence[0], length);
    for (auto& q : quality_values) {
        reader.ReadAlignedBytes(&q[0], q.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Segment::getSequence() const { return sequence; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Segment::getQualities() const { return quality_values; }

// ---------------------------------------------------------------------------------------------------------------------

void Segment::addQualities(std::string&& qv) {
    // Source:
    //  From Section 10.2.16
    // quality_values[rSeq][qs] array shall be a c(read_len[rSeg]), otherwise quality_values[rSeq][qs] shall be a
    // c(1) string containing only one character corresponding to ASCII code 0
    static const std::string NO_QUAL("\0", 1);
    UTILS_DIE_IF(!((qv == NO_QUAL) || (qv.length() == sequence.length())), "QV and sequence lengths do not match");
    quality_values.emplace_back(std::move(qv));
}

// ---------------------------------------------------------------------------------------------------------------------

void Segment::write(util::BitWriter& writer) const {
    writer.WriteAlignedBytes(this->sequence.data(), this->sequence.length());
    for (const auto& a : this->quality_values) {
        writer.WriteAlignedBytes(a.data(), a.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

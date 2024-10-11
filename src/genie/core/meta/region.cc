/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/region.h"
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

// ---------------------------------------------------------------------------------------------------------------------

Region::Region(uint16_t seqid, uint64_t start, uint64_t end, std::vector<record::ClassType> _classes)
    : seq_ID(seqid), classes(std::move(_classes)), start_pos(start), end_pos(end) {}

// ---------------------------------------------------------------------------------------------------------------------

Region::Region(const nlohmann::json& json)
    : seq_ID(json["seq_ID"]), start_pos(json["start_pos"]), end_pos(json["end_pos"]) {
    std::vector<uint8_t> tmp = json["classes"];
    for (const auto& t : tmp) {
        classes.push_back(static_cast<record::ClassType>(t));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Region::toJson() const {
    nlohmann::json ret;
    ret["seq_ID"] = seq_ID;
    ret["start_pos"] = start_pos;
    ret["end_pos"] = end_pos;
    std::vector<uint8_t> tmp;
    for (const auto& t : classes) {
        tmp.push_back(static_cast<uint8_t>(t));
        ret["classes"] = tmp;
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Region::getSeqID() const { return seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<record::ClassType>& Region::getClasses() const { return classes; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Region::getStartPos() const { return start_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Region::getEndPos() const { return end_pos; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

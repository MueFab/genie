/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/label.h"
#include <string>
#include <utility>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(std::string id) : label_ID(std::move(id)), regions() {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(const nlohmann::json& json) : label_ID(json["label_ID"]) {
    for (const auto& r : json["regions"]) {
        regions.emplace_back(r);
    }
    UTILS_DIE_IF(regions.empty(), "Empty label region.");
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Label::toJson() const {
    nlohmann::json ret;
    ret["label_ID"] = label_ID;
    for (const auto& r : regions) {
        ret["regions"].push_back(r.toJson());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::addRegion(Region r) { regions.emplace_back(std::move(r)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Label::getID() const { return label_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Region>& Label::getRegions() const { return regions; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

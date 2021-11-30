/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/label.h"
#include <string>
#include <utility>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(std::string id, Region&& firstRegion) : label_ID(std::move(id)), regions({std::move(firstRegion)}) {}

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

void Label::addRegion(const Region& r) { regions.push_back(r); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Label::geID() const { return label_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Region>& Label::getRegions() const { return regions; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/blockheader/enabled.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta::blockheader {

// ---------------------------------------------------------------------------------------------------------------------

Enabled::Enabled(bool mit, bool cc) : BlockHeader(HeaderType::ENABLED), MIT_flag(mit), CC_mode_flag(cc) {}

// ---------------------------------------------------------------------------------------------------------------------

Enabled::Enabled(const nlohmann::json& json)
    : BlockHeader(HeaderType::ENABLED), MIT_flag(json["MIT_flag"]), CC_mode_flag(json["CC_mode_flag"]) {}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Enabled::toJson() const {
    nlohmann::json ret;
    ret["MIT_flag"] = MIT_flag;
    ret["CC_mode_flag"] = CC_mode_flag;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Enabled::getMITFlag() const { return MIT_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool Enabled::getCCFlag() const { return CC_mode_flag; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta::blockheader

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

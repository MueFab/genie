/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/blockheader/disabled.h"

// ---------------------------------------------------------------------------------------------------------------------




namespace genie::core::meta::blockheader {

// ---------------------------------------------------------------------------------------------------------------------

Disabled::Disabled(bool ordered) : BlockHeader(HeaderType::DISABLED), ordered_blocks_flag(ordered) {}

// ---------------------------------------------------------------------------------------------------------------------

Disabled::Disabled(const nlohmann::json& json)
    : BlockHeader(HeaderType::DISABLED), ordered_blocks_flag(json["ordered_blocks_flag"]) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Disabled::getOrderedFlag() const { return ordered_blocks_flag; }

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Disabled::toJson() const {
    nlohmann::json ret;
    ret["ordered_blocks_flag"] = ordered_blocks_flag;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genie::core::meta::blockheader




// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

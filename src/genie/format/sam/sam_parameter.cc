/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/sam_parameter.h"
#include <cassert>
#include <filesystem>  // NOLINT
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread> //NOLINT
#include <vector>
#include "cli11/CLI11.hpp"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

Config::~Config() = default;

bool compare(const genie::core::record::Record& r1, const genie::core::record::Record& r2) {
    if (r1.getAlignments().empty()) {
        return false;
    }
    if (r2.getAlignments().empty()) {
        return true;
    }
    if (r1.getAlignmentSharedData().getSeqID() != r2.getAlignmentSharedData().getSeqID()) {
        return r1.getAlignmentSharedData().getSeqID() < r2.getAlignmentSharedData().getSeqID();
    }
    return r1.getAlignments().front().getPosition() < r2.getAlignments().front().getPosition();
}

}  // namespace genie::format::sam


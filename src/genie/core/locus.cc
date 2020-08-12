/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "locus.h"
#include <genie/util/runtime-exception.h>
#include <genie/util/string-helpers.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Locus::getRef() const { return refName; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Locus::getStart() const { return start; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Locus::getEnd() const { return end; }

// ---------------------------------------------------------------------------------------------------------------------

bool Locus::positionPresent() const { return posPresent; }

// ---------------------------------------------------------------------------------------------------------------------

Locus::Locus(std::string _ref) : refName(std::move(_ref)), posPresent(false), start(0), end(0) {}

// ---------------------------------------------------------------------------------------------------------------------

Locus::Locus(std::string _ref, uint32_t _start, uint32_t _end)
    : refName(std::move(_ref)), posPresent(true), start(_start), end(_end) {
    UTILS_DIE_IF(end < start, "Locus ends before start");
}

// ---------------------------------------------------------------------------------------------------------------------

Locus Locus::fromString(const std::string& string) {
    auto tok = util::tokenize(string, ':');
    constexpr size_t NUM_START_END = 2;
    if (tok.size() == 1) {
        return Locus(tok.front());  // Sequence name only
    } else if (tok.size() == NUM_START_END) {
        auto pos = util::tokenize(tok.back(), '-');  // Sequence + position
        UTILS_DIE_IF(pos.size() != NUM_START_END, "Invalid locus");
        return Locus(tok.front(), static_cast<uint32_t>(std::stoi(pos[0])), static_cast<uint32_t>(std::stoi(pos[1])));
    } else {
        UTILS_DIE("Invalid locus");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Locus::toString() const {
    return posPresent ? refName + ':' + std::to_string(start) + '-' + std::to_string(end) : refName;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
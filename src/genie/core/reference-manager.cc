/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reference-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

std::string ReferenceManager::getSequence(const std::string& name, uint64_t _start, uint64_t _end) const {
    auto it = refs.find(name);
    if (it == refs.end()) {
        return "";
    }
    uint64_t position = _start;
    std::string ret;
    ret.reserve(_end - _start);
    for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
        if (position >= _end) {
            return ret;
        }
        if (it2->get()->getStart() <= position && it2->get()->getEnd() > position) {
            ret += it2->get()->getSequence(position, std::min(it2->get()->getEnd(), _end));
            position = it2->get()->getEnd();
            it2 = it->second.begin();
        }
    }
    return "";
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::registerRef(std::unique_ptr<Reference> ref) {
    refs[ref->getName()].emplace_back(std::move(ref));
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::registerRef(std::vector<std::unique_ptr<Reference>>&& ref) {
    auto vec = std::move(ref);
    for (auto& v : vec) {
        registerRef(std::move(v));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
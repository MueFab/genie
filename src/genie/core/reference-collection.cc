/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reference-collection.h"

#include <limits>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

std::string ReferenceCollection::getSequence(const std::string& name, uint64_t _start, uint64_t _end) const {
    auto it = refs.find(name);
    if (it == refs.end()) {
        return "";
    }
    uint64_t position = _start;
    std::string ret;
    ret.reserve(_end - _start);
    while (true) {
        size_t nearest = std::numeric_limits<size_t>::max();
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            if (position >= _end) {
                return ret;
            }

            if (it2->get()->getStart() > position) {
                nearest = std::min<size_t>(nearest, it2->get()->getStart());
            }

            if (it2->get()->getStart() <= position && it2->get()->getEnd() > position) {
                nearest = std::numeric_limits<size_t>::max();
                ret += it2->get()->getSequence(position, std::min<size_t>(it2->get()->getEnd(), _end));
                position = it2->get()->getEnd();
                it2 = it->second.begin();
            }
        }

        if (position >= _end) {
            return ret;
        }

        if (nearest == std::numeric_limits<size_t>::max()) {
            ret += std::string(_end - position, 'N');
            return ret;
        }
        ret += std::string(nearest - position, 'N');
        position = nearest;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceCollection::registerRef(std::unique_ptr<Reference> ref) {
    refs[ref->getName()].emplace_back(std::move(ref));
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceCollection::registerRef(std::vector<std::unique_ptr<Reference>>&& ref) {
    auto vec = std::move(ref);
    for (auto& v : vec) {
        registerRef(std::move(v));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<size_t, size_t>> ReferenceCollection::getCoverage(const std::string& name) const {
    std::vector<std::pair<size_t, size_t>> ret;
    auto it = refs.find(name);
    if (it == refs.end()) {
        return ret;
    }
    for (const auto& s : it->second) {
        ret.emplace_back(s->getStart(), s->getEnd());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> ReferenceCollection::getSequences() const {
    std::vector<std::string> ret;
    for (const auto& s : refs) {
        ret.emplace_back(s.first);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "classifier-regroup.h"
#include "constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

ClassifierRegroup::ClassifierRegroup(size_t _auSize) : auSize(_auSize), flushing(false) {
    classes.resize(uint8_t(record::ClassType::CLASS_U));
    for (auto& c : classes) {
        c.emplace_back();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

record::Chunk ClassifierRegroup::getChunk() {
    for (auto& c : classes) {
        if (c.size() > 1 || (!c.front().empty() && flushing)) {
            record::Chunk chunk = std::move(c.front());
            c.erase(c.begin());
            if (c.empty()) {
                c.emplace_back();
            }
            return chunk;
        }
    }
    if (flushing) {
        flushing = false;
    }
    return record::Chunk();
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::add(record::Chunk&& c) {
    record::Chunk chunk = std::move(c);
    for (auto& r : chunk) {
        auto classtype = classify(r);
        r.setClassType(classtype);
        if (r.getClassID() < record::ClassType::CLASS_HM && r.getAlignmentSharedData().getSeqID() != currentSeq) {
            currentSeq = r.getAlignmentSharedData().getSeqID();
            for (size_t i = 0; i < uint8_t(record::ClassType::CLASS_HM); ++i) {
                classes[i].emplace_back();
            }
        }
        if (classes[uint8_t(classtype)].back().size() >= auSize) {
            classes[uint8_t(classtype)].emplace_back();
        }
        classes[uint8_t(classtype)].back().emplace_back(std::move(r));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::flush() { flushing = true; }

// ---------------------------------------------------------------------------------------------------------------------

bool ClassifierRegroup::isFlushing() const { return flushing; }

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::classifyECigar(const std::string& cigar) {
    for (const auto& c : cigar) {
        if (c == '+' || c == '-') {
            return record::ClassType::CLASS_I;
        }
        if (getAlphabetProperties(AlphabetID::ACGTN).isIncluded(c)) {
            return record::ClassType::CLASS_M;
        }
    }
    return record::ClassType::CLASS_P;
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::classifySeq(const std::string& seq) {
    for (const auto& c : seq) {
        if (c == 'N') {
            return record::ClassType::CLASS_N;
        }
    }
    return record::ClassType::CLASS_P;
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::classify(const record::Record& r) {
    if (r.getAlignments().empty()) {
        return record::ClassType::CLASS_U;
    }
    if (r.getAlignments().front().getAlignmentSplits().size() + 1 < r.getSegments().size()) {
        return record::ClassType::CLASS_HM;
    }

    record::ClassType highestClass = record::ClassType::CLASS_P;
    for (const auto& a : r.getAlignments().front().getAlignmentSplits()) {
        if (a->getType() != record::AlignmentSplit::Type::SAME_REC) {
            continue;
        }
        auto& split = dynamic_cast<const record::alignment_split::SameRec&>(*a);
        highestClass = std::max(highestClass, classifyECigar(split.getAlignment().getECigar()));
        if (highestClass == record::ClassType::CLASS_I) {
            return highestClass;
        }
    }

    if (highestClass >= record::ClassType::CLASS_M) {
        return highestClass;
    }

    for (const auto& s : r.getSegments()) {
        highestClass = std::max(highestClass, classifySeq(s.getSequence()));
        if (highestClass >= record::ClassType::CLASS_N) {
            return highestClass;
        }
    }

    return highestClass;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
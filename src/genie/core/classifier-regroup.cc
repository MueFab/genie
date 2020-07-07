/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "classifier-regroup.h"
#include <genie/util/watch.h>
#include "cigar-tokenizer.h"
#include "constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

ClassifierRegroup::ClassifierRegroup(size_t _auSize) : auSize(_auSize) {
    currentChunks.resize(2);
    for (auto& c : currentChunks) {
        c.resize((uint8_t)record::ClassType::CLASS_U);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

record::Chunk ClassifierRegroup::getChunk() {
    auto ret = record::Chunk();
    if (finishedChunks.empty()) {
        return ret;
    }

    ret = std::move(finishedChunks.front());
    finishedChunks.erase(finishedChunks.begin());
    return ret;
}

record::ClassType fineClassifierECigar(const std::string& ref, const std::string& seq,
                                       const std::string& ecigar) {
    record::ClassType classtype = record::ClassType::CLASS_P;

    auto classChecker = [&classtype, &ref, &seq](uint8_t cigar, const util::StringView& bs,
                                                     const util::StringView& rs) -> bool {
        // Possible mismatches not encoded in ecigar
        if (cigar == '=') {
            for (size_t i = 0; i < bs.length(); ++i) {
                // Mismatch
                if (*(bs.begin() + i) != *(rs.begin() + i)) {
                    if (*(bs.begin() + i) == 'N') {
                        classtype = record::ClassType::CLASS_N;
                    } else {
                        // Abort, class must be M
                        classtype = record::ClassType::CLASS_M;
                        return false;
                    }
                }
            }
            // Mismatch already encoded in ecigar
        } else if (core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar)) {
            if (cigar != 'N') {
                classtype = record::ClassType::CLASS_M;
                return false;
            }
            classtype = record::ClassType::CLASS_N;
            return true;
        }
        return true;
    };

    core::CigarTokenizer::tokenize(ecigar, core::getECigarInfo(), classChecker);
    return classtype;
}

record::ClassType fineClassifierRecord(const ReferenceManager::ReferenceExcerpt& record_reference,
                                       const core::record::Record& rec) {
    record::ClassType ret = record::ClassType::CLASS_P;
    auto segment_it = rec.getSegments().begin();

    std::string ref =
        record_reference.getString(rec.getPosition(0, 0), rec.getPosition(0, 0) + rec.getMappedLength(0, 0));

    ret = std::max(ret, fineClassifierECigar(ref, segment_it->getSequence(),
                                             rec.getAlignments().front().getAlignment().getECigar()));

    for (size_t i = 0; i < rec.getAlignments().front().getAlignmentSplits().size(); ++i) {
        if (ret == core::record::ClassType::CLASS_M) {
            return ret;
        }
        segment_it++;

        auto pos = rec.getPosition(0, i + 1);

        ref = record_reference.getString(pos, pos + rec.getMappedLength(0, i + 1));

        ret = std::max(ret,
                       fineClassifierECigar(ref, segment_it->getSequence(), dynamic_cast<core::record::alignment_split::SameRec&>(*rec.getAlignments().front().getAlignmentSplits()[i]).getAlignment().getECigar()));
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::add(record::Chunk&& c) {
    record::Chunk chunk = std::move(c);
    auto chunk_reference =
        refMgr->load(chunk.getRef().getRefName(), chunk.getRef().getGlobalStart(), chunk.getRef().getGlobalEnd());

    if (chunk.getRef().getRefName() != currentSeq) {
        currentSeq = chunk.getRef().getRefName();
        for (auto& refblock : currentChunks) {
            for (auto& pairblock : refblock) {
                for (auto& classblock : pairblock) {
                    if (classblock.getData().empty() ||
                        classblock.getData().front().getClassID() == record::ClassType::CLASS_U) {
                        continue;
                    }
                    finishedChunks.push_back(std::move(classblock));
                }
            }
        }
    }

    for (auto& r : chunk.getData()) {
        auto classtype = coarseClassify(r);  // Only look at the ecigar for first classification
        bool paired = r.getNumberOfTemplateSegments() > 1;
        bool refBased = false;

        // Unaligned reads can't be ref based, otherwise check if reference available
        if (classtype != core::record::ClassType::CLASS_U) {
            refBased = isCovered(r);
        }
        if (refBased && classtype == record::ClassType::CLASS_M) {
            classtype = fineClassifierRecord(chunk_reference, r);
        }
        r.setClassType(classtype);

        currentChunks[refBased][paired][(uint8_t)classtype].getData().push_back(r);
        if (currentChunks[refBased][paired][(uint8_t)classtype].getData().size() == auSize) {
            finishedChunks.push_back(std::move(currentChunks[refBased][paired][(uint8_t)classtype]));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::flush() {
    for (auto& refblock : currentChunks) {
        for (auto& pairblock : refblock) {
            for (auto& classblock : pairblock) {
                if (classblock.getData().empty()) {
                    continue;
                }
                finishedChunks.push_back(std::move(classblock));
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::classifyECigar(const std::string& cigar) {
    auto highest = record::ClassType::CLASS_P;
    for (const auto& c : cigar) {
        if (c == '+' || c == '-') {
            return record::ClassType::CLASS_I;
        }
        if (getAlphabetProperties(AlphabetID::ACGTN).isIncluded(c)) {
            highest = record::ClassType::CLASS_M;
        }
    }
    return highest;
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

record::ClassType ClassifierRegroup::coarseClassify(const record::Record& r) {
    if (r.getAlignments().empty()) {
        return record::ClassType::CLASS_U;
    }
    if (r.getAlignments().front().getAlignmentSplits().size() + 1 < r.getSegments().size()) {
        return record::ClassType::CLASS_HM;
    }

    record::ClassType highestClass = record::ClassType::CLASS_M;
    highestClass = std::max(highestClass, classifyECigar(r.getAlignments().front().getAlignment().getECigar()));
    if (highestClass == record::ClassType::CLASS_I) {
        return highestClass;
    }
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
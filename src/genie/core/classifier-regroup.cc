/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/classifier-regroup.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/constants.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

bool ClassifierRegroup::isCovered(size_t start, size_t end) const {
    size_t position = start;

    for (auto it = currentSeqCoverage.begin(); it != currentSeqCoverage.end(); ++it) {
        if (position >= it->first && position < it->second) {
            position = it->second - 1;
            it = currentSeqCoverage.begin();
        }
        if (position >= (end - 1)) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ClassifierRegroup::isCovered(const core::record::Record& r) const {
    for (size_t i = 0; i <= r.getAlignments().front().getAlignmentSplits().size(); ++i) {
        auto pos = r.getPosition(0, i);
        if (!isCovered(pos, pos + r.getMappedLength(0, i))) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::push(bool refBased, bool paired, core::record::ClassType classtype, core::record::Record& r) {
    auto& chunk = currentChunks[refBased][paired][(uint8_t)classtype - 1];
    if (chunk.getRef().getRefName().empty()) {
        auto pos = r.getPosition(0, 0);
        chunk.getRef() = ReferenceManager::ReferenceExcerpt(currentSeq, pos, pos + r.getMappedLength(0, 0));
        for (size_t i = 0; i < r.getAlignments().front().getAlignmentSplits().size(); ++i) {
            pos = r.getPosition(0, 1);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::queueFinishedChunk(core::record::Chunk& data) {
    if (!data.getRef().isEmpty()) {
        if (refMode == RefMode::RELEVANT) {
            for (size_t i = data.getRef().getGlobalStart() / refMgr->getChunkSize();
                 i <= (data.getRef().getGlobalEnd() - 1) / refMgr->getChunkSize(); ++i) {
                if (isWritten(data.getRef().getRefName(), i)) {
                    continue;
                }
                refState.at(data.getRef().getRefName()).at(i) = 1;

                if (rawRefMode) {
                    size_t length = refMgr->getLength(data.getRef().getRefName());
                    data.addRefToWrite(i * refMgr->getChunkSize(), std::min((i + 1) * refMgr->getChunkSize(), length));
                } else {
                    size_t length = refMgr->getLength(data.getRef().getRefName());
                    core::record::Chunk refChunk;
                    refChunk.setReferenceOnly(true);
                    refChunk.setRefID(refMgr->ref2ID(data.getRef().getRefName()));
                    refChunk.getRef() = data.getRef();
                    core::record::Record rec(1, core::record::ClassType::CLASS_U, "", "", 0);
                    std::string seq = *data.getRef().getChunkAt(i * refMgr->getChunkSize());
                    if ((i + 1) * refMgr->getChunkSize() > length) {
                        seq = seq.substr(0, length - i * refMgr->getChunkSize());
                    }
                    core::record::Segment segment(std::move(seq));
                    rec.addSegment(std::move(segment));
                    refChunk.getData().push_back(std::move(rec));
                    finishedChunks.push_back(std::move(refChunk));
                }
            }
        }

        data.setRefID(refMgr->ref2ID(data.getRef().getRefName()));
    }
    finishedChunks.push_back(std::move(data));
}

// ---------------------------------------------------------------------------------------------------------------------

bool ClassifierRegroup::isWritten(const std::string& ref, size_t index) {
    if (refState.find(ref) == refState.end()) {
        refState.insert(std::make_pair(ref, std::vector<uint8_t>(1, 0)));
    }
    if (refState.at(ref).size() <= index) {
        refState.at(ref).resize(index + 1, 0);
    }
    return refState.at(ref).at(index);
}

// ---------------------------------------------------------------------------------------------------------------------

ClassifierRegroup::ClassifierRegroup(size_t _auSize, ReferenceManager* rfmgr, RefMode mode, bool raw_ref)
    : refMgr(rfmgr), currentSeqID(0), auSize(_auSize), refMode(mode), rawRefMode(raw_ref) {
    currentChunks.resize(2);
    for (auto& c : currentChunks) {
        c.resize(2);
        for (auto& c2 : c) {
            c2.resize((uint8_t)record::ClassType::CLASS_U);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

record::Chunk ClassifierRegroup::getChunk() {
    if (refMode == RefMode::FULL) {
        auto seqvec = refMgr->getSequences();
        while (true) {
            if (refModeFullSeqID != seqvec.size()) {
                auto seq = seqvec.at(refModeFullSeqID);
                auto cov_vec = refMgr->getCoverage(seq);
                auto cov = cov_vec.at(refModeFullCovID);
                size_t chunkOffset = cov.first / refMgr->getChunkSize();

                if (!isWritten(seq, chunkOffset + refModeFullChunkID)) {
                    core::record::Chunk refChunk;

                    std::cerr << "Writing ref " << seq << " ["
                              << std::max(cov.first, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize())
                              << ", "
                              << std::min(cov.second, (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize())
                              << "]" << std::endl;

                    if (rawRefMode) {
                        refChunk.addRefToWrite(
                            std::max(cov.first, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize()),
                            std::min(cov.second, (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize()));
                        refChunk.getRef() = refMgr->load(
                            seq, std::max(cov.first, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize()),
                            std::min(cov.second, (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize()));
                        refChunk.setRefID(refMgr->ref2ID(seq));
                    } else {
                        refChunk.setReferenceOnly(true);
                        refChunk.setRefID(refMgr->ref2ID(seq));
                        refChunk.getRef() = refMgr->load(
                            seq, std::max(cov.first, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize()),
                            std::min(cov.second, (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize()));
                        core::record::Record rec(1, core::record::ClassType::CLASS_U, "", "", 0);
                        std::string ref_seq =
                            *refChunk.getRef().getChunkAt((chunkOffset + refModeFullChunkID) * refMgr->getChunkSize());
                        if (ref_seq.empty()) {
                            std::cerr << "empty" << std::endl;
                        }
                        if (cov.first > (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize()) {
                            ref_seq =
                                ref_seq.substr(cov.first - (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize());
                        }
                        if (cov.second < (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize()) {
                            ref_seq = ref_seq.substr(
                                0, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize() - cov.second);
                        }
                        core::record::Segment segment(std::move(ref_seq));
                        rec.addSegment(std::move(segment));
                        refChunk.getData().push_back(std::move(rec));
                    }

                    refModeFullChunkID++;
                    if (refModeFullChunkID > ((cov.second - 1) / refMgr->getChunkSize())) {
                        refModeFullCovID++;
                        refModeFullChunkID = 0;
                    }
                    if (refModeFullCovID == cov_vec.size()) {
                        refModeFullCovID = 0;
                        refModeFullSeqID++;
                    }

                    return refChunk;
                } else {
                    refModeFullChunkID++;
                    if (refModeFullChunkID > ((cov.second - 1) / refMgr->getChunkSize())) {
                        refModeFullCovID++;
                        refModeFullChunkID = 0;
                    }
                    if (refModeFullCovID == cov_vec.size()) {
                        refModeFullCovID = 0;
                        refModeFullSeqID++;
                    }
                }
            } else {
                break;
            }
        }
    }

    auto ret = record::Chunk();
    if (finishedChunks.empty()) {
        return ret;
    }

    ret = std::move(finishedChunks.front());
    finishedChunks.erase(finishedChunks.begin());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::fineClassifierECigar(const std::string& ref, const std::string& seq,
                                                          const std::string& ecigar) {
    record::ClassType classtype = record::ClassType::CLASS_P;

    auto classChecker = [&classtype, &ref, &seq](uint8_t cigar, const util::StringView& _bs,
                                                 const util::StringView& _rs) -> bool {
        auto bs = _bs.deploy(seq.data());
        auto rs = _rs.deploy(ref.data());
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

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::fineClassifierRecord(ReferenceManager::ReferenceExcerpt& record_reference,
                                                          const core::record::Record& rec, bool loadonly) {
    record::ClassType ret = record::ClassType::CLASS_P;
    auto segment_it = rec.getSegments().begin();

    auto pos_s = rec.getPosition(0, 0);
    auto pos_e = pos_s + rec.getMappedLength(0, 0);

    if (record_reference.getRefName().empty()) {
        record_reference = ReferenceManager::ReferenceExcerpt(currentSeq, pos_s, pos_e);
    } else {
        record_reference.extend(pos_e);
    }

    record_reference.mapSection(pos_s, pos_e, refMgr);

    std::string ref = record_reference.getString(pos_s, pos_e);

    if (!loadonly) {
        ret = std::max(ret, fineClassifierECigar(ref, segment_it->getSequence(),
                                                 rec.getAlignments().front().getAlignment().getECigar()));
    }

    for (size_t i = 0; i < rec.getAlignments().front().getAlignmentSplits().size(); ++i) {
        if (ret == core::record::ClassType::CLASS_M) {
            return ret;
        }
        segment_it++;

        pos_s = rec.getPosition(0, i + 1);
        pos_e = pos_s + rec.getMappedLength(0, i + 1);

        record_reference.extend(pos_e);

        record_reference.mapSection(pos_s, pos_e, refMgr);

        ref = record_reference.getString(pos_s, pos_e);

        if (!loadonly) {
            ret = std::max(ret, fineClassifierECigar(ref, segment_it->getSequence(),
                                                     dynamic_cast<core::record::alignment_split::SameRec&>(
                                                         *rec.getAlignments().front().getAlignmentSplits()[i])
                                                         .getAlignment()
                                                         .getECigar()));
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::add(record::Chunk&& c) {
    record::Chunk chunk = std::move(c);

    // New reference
    if (chunk.getRefID() != currentSeqID) {
        currentSeq = chunk.getRef().getRefName();
        currentSeqID = static_cast<uint16_t>(chunk.getRefID());
        currentSeqCoverage = refMgr->getCoverage(currentSeq);
        for (auto& refblock : currentChunks) {
            for (auto& pairblock : refblock) {
                for (auto& classblock : pairblock) {
                    if (classblock.getData().empty() ||
                        classblock.getData().front().getClassID() == record::ClassType::CLASS_U) {
                        continue;
                    }
                    queueFinishedChunk(classblock);
                }
            }
        }
    }

    for (auto& r : chunk.getData()) {
        auto classtype = r.getClassID();  // Only look at the ecigar for first classification
        bool paired = r.getNumberOfTemplateSegments() > 1;
        bool refBased = false;

        ReferenceManager::ReferenceExcerpt record_reference;

        // Unaligned reads can't be ref based, otherwise check if reference available
        if (classtype != core::record::ClassType::CLASS_U) {
            refBased = isCovered(r);
        }
 /*       if (refBased) {
            // Load reference and do detailed classification
            if (classtype == record::ClassType::CLASS_M) {
                classtype = fineClassifierRecord(record_reference, r, false);
            } else {
                fineClassifierRecord(record_reference, r, true);
            }
        }
        r.setClassType(classtype);*/

        currentChunks[refBased][paired][(uint8_t)classtype - 1].getData().push_back(r);
        currentChunks[refBased][paired][(uint8_t)classtype - 1].getRef().merge(record_reference);
        if (currentChunks[refBased][paired][(uint8_t)classtype - 1].getData().size() == auSize) {
            auto& classblock = currentChunks[refBased][paired][(uint8_t)classtype - 1];
            queueFinishedChunk(classblock);
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
                queueFinishedChunk(classblock);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::classifyECigar(const std::string& cigar) {
    auto highest = record::ClassType::CLASS_P;
    for (const auto& c : cigar) {
        if (c == '+' || c == '-' || c == ')' || c == ']') {
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

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/classifier-regroup.h"
#include <algorithm>
#include <fstream>
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
    for (size_t i = 0; i < r.getAlignments().front().getAlignmentSplits().size() + 1; ++i) {
        if (i > 0 && r.getAlignments().front().getAlignmentSplits()[i - 1]->getType() !=
                         genie::core::record::AlignmentSplit::Type::SAME_REC) {
            continue;
        }
        auto pos = r.getPosition(0, i);
        if (!isCovered(pos, pos + r.getMappedLength(0, i))) {
            return false;
        }
    }
    return true;
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
    : refMgr(rfmgr), currentSeqID(-1), auSize(_auSize), refMode(mode), rawRefMode(raw_ref) {
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

void ClassifierRegroup::add(record::Chunk&& c) {
    record::Chunk chunk = std::move(c);

    // New reference
    if (chunk.getRefID() != static_cast<uint16_t>(currentSeqID)) {
        currentSeqID = static_cast<uint16_t>(chunk.getRefID());
        if (refMgr->refKnown(currentSeqID)) {
            currentSeqCoverage = refMgr->getCoverage(refMgr->ID2Ref(currentSeqID));
        } else {
            currentSeqCoverage = std::vector<std::pair<size_t, size_t>>();
        }
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
            if (refBased) {
                size_t end = 0;
                if (r.getAlignments().front().getAlignmentSplits().empty() ||
                    r.getAlignments().front().getAlignmentSplits().front()->getType() !=
                        core::record::AlignmentSplit::Type::SAME_REC) {
                    end = r.getAlignments().front().getPosition() + r.getMappedLength(0, 0);
                } else {
                    end = r.getAlignments().front().getPosition() + r.getMappedLength(0, 1) +
                          dynamic_cast<const core::record::alignment_split::SameRec&>(
                              *r.getAlignments().front().getAlignmentSplits().front())
                              .getDelta();
                }
                record_reference = this->refMgr->load(refMgr->ID2Ref(r.getAlignmentSharedData().getSeqID()),
                                                      r.getAlignments().front().getPosition(), end);
            }
        }

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

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

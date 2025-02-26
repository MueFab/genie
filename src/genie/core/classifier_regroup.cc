/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/cigar_tokenizer.h"
#include "genie/core/classifier_regroup.h"
#include "genie/core/constants.h"
#include "genie/util/stop_watch.h"

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
        if (i > 0 && r.getAlignments().front().getAlignmentSplits()[i - 1]->GetType() !=
                         genie::core::record::AlignmentSplit::Type::kSameRec) {
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
    if (!data.GetRef().isEmpty()) {
        if (refMode == RefMode::RELEVANT) {
            for (size_t i = data.GetRef().getGlobalStart() / refMgr->getChunkSize();
                 i <= (data.GetRef().getGlobalEnd() - 1) / refMgr->getChunkSize(); ++i) {
                if (isWritten(data.GetRef().getRefName(), i)) {
                    continue;
                }
                refState.at(data.GetRef().getRefName()).at(i) = 1;

                if (rawRefMode) {
                    size_t length = refMgr->getLength(data.GetRef().getRefName());
                    data.AddRefToWrite(i * refMgr->getChunkSize(), std::min((i + 1) * refMgr->getChunkSize(), length));
                } else {
                    size_t length = refMgr->getLength(data.GetRef().getRefName());
                    core::record::Chunk refChunk;
                    refChunk.SetReferenceOnly(true);
                    refChunk.SetRefId(refMgr->ref2ID(data.GetRef().getRefName()));
                    refChunk.GetRef() = data.GetRef();
                    core::record::Record rec(1, core::record::ClassType::kClassU, "", "", 0);
                    std::string seq = *data.GetRef().getChunkAt(i * refMgr->getChunkSize());
                    if ((i + 1) * refMgr->getChunkSize() > length) {
                        seq = seq.substr(0, length - i * refMgr->getChunkSize());
                    }
                    core::record::Segment segment(std::move(seq));
                    rec.addSegment(std::move(segment));
                    refChunk.GetData().push_back(std::move(rec));
                    finishedChunks.push_back(std::move(refChunk));
                }
            }
        }

        data.SetRefId(refMgr->ref2ID(data.GetRef().getRefName()));
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
            c2.resize((uint8_t)record::ClassType::kClassU);
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
                        refChunk.AddRefToWrite(
                            std::max(cov.first, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize()),
                            std::min(cov.second, (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize()));
                        refChunk.GetRef() = refMgr->load(
                            seq, std::max(cov.first, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize()),
                            std::min(cov.second, (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize()));
                        refChunk.SetRefId(refMgr->ref2ID(seq));
                    } else {
                        refChunk.SetReferenceOnly(true);
                        refChunk.SetRefId(refMgr->ref2ID(seq));
                        refChunk.GetRef() = refMgr->load(
                            seq, std::max(cov.first, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize()),
                            std::min(cov.second, (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize()));
                        core::record::Record rec(1, core::record::ClassType::kClassU, "", "", 0);
                        std::string ref_seq =
                            *refChunk.GetRef().getChunkAt((chunkOffset + refModeFullChunkID) * refMgr->getChunkSize());
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
                        refChunk.GetData().push_back(std::move(rec));
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

#define AU_DEBUG_WRITE 0
#if AU_DEBUG_WRITE
    static int i = 0;
    std::ofstream tmpOut("AU_" + std::to_string(i++) + ".mgrec");
    util::BitWriter bw(&tmpOut);
    for (const auto& r : ret.getData()) {
        r.write(bw);
    }
#endif
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::add(record::Chunk&& c) {
    record::Chunk chunk = std::move(c);
    bool movedStats = false;

    // New reference
    if (chunk.GetRefId() != static_cast<uint16_t>(currentSeqID)) {
        currentSeqID = static_cast<uint16_t>(chunk.GetRefId());
        if (refMgr->refKnown(currentSeqID)) {
            currentSeqCoverage = refMgr->getCoverage(refMgr->ID2Ref(currentSeqID));
        } else {
            currentSeqCoverage = std::vector<std::pair<size_t, size_t>>();
        }
        for (auto& refblock : currentChunks) {
            for (auto& pairblock : refblock) {
                for (auto& classblock : pairblock) {
                    if (classblock.GetData().empty() ||
                        classblock.GetData().front().getClassID() == record::ClassType::kClassU) {
                        continue;
                    }
                    queueFinishedChunk(classblock);
                }
            }
        }
    }

    for (auto& r : chunk.GetData()) {
        auto classtype = r.getClassID();  // Only look at the ecigar for first classification
        bool paired = r.getNumberOfTemplateSegments() > 1;
        bool refBased = false;

        if (r.getClassID() == core::record::ClassType::kClassU &&
            r.getNumberOfTemplateSegments() != r.getSegments().size()) {
            current_unpaired_u_Chunk.GetData().emplace_back(std::move(r));
            if (current_unpaired_u_Chunk.GetData().size() >= auSize) {
                queueFinishedChunk(current_unpaired_u_Chunk);
            }
            continue;
        }

        ReferenceManager::ReferenceExcerpt record_reference;

        // Unaligned reads can't be ref based, otherwise check if reference available
        if (classtype != core::record::ClassType::kClassU) {
            refBased = isCovered(r);
            if (refBased) {
                size_t end = 0;
                if (r.getAlignments().front().getAlignmentSplits().empty() ||
                    r.getAlignments().front().getAlignmentSplits().front()->GetType() !=
                        core::record::AlignmentSplit::Type::kSameRec) {
                    end = r.getAlignments().front().getPosition() + r.getMappedLength(0, 0);
                } else {
                    end = r.getAlignments().front().getPosition() + r.getMappedLength(0, 1) +
                          dynamic_cast<const core::record::alignment_split::SameRec&>(
                              *r.getAlignments().front().getAlignmentSplits().front())
                              .getDelta();
                    end = std::max(static_cast<uint64_t>(end),
                                   r.getAlignments().front().getPosition() + r.getMappedLength(0, 0));
                }
                record_reference = this->refMgr->load(refMgr->ID2Ref(r.getAlignmentSharedData().getSeqID()),
                                                      r.getAlignments().front().getPosition(), end);
            }
        }

        if (currentChunks[refBased][paired][(uint8_t)classtype - 1].GetData().empty()) {
            currentChunks[refBased][paired][(uint8_t)classtype - 1].GetRef() = record_reference;
        } else {
            currentChunks[refBased][paired][(uint8_t)classtype - 1].GetRef().merge(record_reference);
        }
        if (!movedStats) {
            currentChunks[refBased][paired][(uint8_t)classtype - 1].GetStats().add(chunk.GetStats());
            movedStats = true;
        }
        currentChunks[refBased][paired][(uint8_t)classtype - 1].GetData().push_back(r);
        if (currentChunks[refBased][paired][(uint8_t)classtype - 1].GetData().size() == auSize) {
            auto& classblock = currentChunks[refBased][paired][(uint8_t)classtype - 1];
            queueFinishedChunk(classblock);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::Flush() {
    for (auto& refblock : currentChunks) {
        for (auto& pairblock : refblock) {
            for (auto& classblock : pairblock) {
                if (classblock.GetData().empty()) {
                    continue;
                }
                queueFinishedChunk(classblock);
            }
        }
    }
    if (!current_unpaired_u_Chunk.GetData().empty()) {
        queueFinishedChunk(current_unpaired_u_Chunk);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

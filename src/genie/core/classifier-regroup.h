/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CLASSIFIERREGROUP_H
#define GENIE_CLASSIFIERREGROUP_H

// ---------------------------------------------------------------------------------------------------------------------

#include "classifier.h"
#include "record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ClassifierRegroup : public Classifier {
   private:

    std::vector<record::Chunk> finishedChunks;

    using ClassBlock = std::vector<record::Chunk>;  //!<
    using PairedBlock = std::vector<ClassBlock>;
    using RefNoRefBlock = std::vector<PairedBlock>;
    RefNoRefBlock currentChunks;                      //!<
    ReferenceManager* refMgr;
    std::string currentSeq;                            //!<
    std::vector<std::pair<size_t, size_t>> currentSeqCoverage; //!<
    std::map<std::string, std::vector<uint8_t>> refState;

    size_t auSize;                                  //!<
    core::stats::PerfStats stats;

    bool isCovered(size_t start, size_t end) const{
        size_t position = start;

        for(auto it = currentSeqCoverage.begin(); it != currentSeqCoverage.end(); ++it) {
            if(position >= it->first && position < it->second) {
                position = it->second - 1;
                it = currentSeqCoverage.begin();
            }
            if(position >= (end - 1)) {
                return true;
            }
        }
        return false;
    }

    bool isCovered(const core::record::Record& r) const{
        for(size_t i = 0; i <= r.getAlignments().front().getAlignmentSplits().size(); ++i) {
            auto pos = r.getPosition(0, i);
            if(!isCovered(pos, pos + r.getMappedLength(0, i))) {
                return false;
            }
        }
        return true;
    }

   void push(bool refBased, bool paired, core::record::ClassType classtype, core::record::Record& r){
       auto& chunk = currentChunks[refBased][paired][(uint8_t)classtype - 1];
       if(chunk.getRef().getRefName().empty()) {
           auto pos = r.getPosition(0,0);
           chunk.getRef() = ReferenceManager::ReferenceExcerpt(currentSeq, pos, pos + r.getMappedLength(0, 0));
           for(size_t i = 0; i < r.getAlignments().front().getAlignmentSplits().size(); ++i) {
               pos = r.getPosition(0, 1);
           }
       }
   }

    void queueFinishedChunk(core::record::Chunk& data) {
        const bool RAW_REFERENCE = true;

        if(!data.getRef().isEmpty()) {
            for (size_t i = data.getRef().getGlobalStart() / refMgr->getChunkSize();
                 i <= (data.getRef().getGlobalEnd() - 1) / refMgr->getChunkSize(); ++i) {
                if (isWritten(data.getRef().getRefName(), i)) {
                    continue;
                }
                refState.at(data.getRef().getRefName()).at(i) = 1;

                if (RAW_REFERENCE) {
                    data.addRefToWrite(i * refMgr->getChunkSize(), (i + 1) * refMgr->getChunkSize());
                } else {
                    core::record::Chunk refChunk;
                    refChunk.setReferenceOnly(true);
                    refChunk.setRefID(refMgr->ref2ID(data.getRef().getRefName()));
                    refChunk.getRef() = data.getRef();
                    core::record::Record rec(1, core::record::ClassType::CLASS_U, "", "", 0);
                    std::string seq = *data.getRef().getChunkAt(i * refMgr->getChunkSize());
                    core::record::Segment segment(std::move(seq));
                    rec.addSegment(std::move(segment));
                    refChunk.getData().push_back(std::move(rec));
                    finishedChunks.push_back(std::move(refChunk));
                }
            }

            data.setRefID(refMgr->ref2ID(data.getRef().getRefName()));
        }
        finishedChunks.push_back(std::move(data));
    }

   public:

    bool isWritten(const std::string& ref, size_t index) {
        if(refState.find(ref) == refState.end()) {
            refState.insert(std::make_pair(ref, std::vector<uint8_t>(1, 0)));
        }
        if(refState.at(ref).size() <= index) {
            refState.at(ref).resize(index + 1, 0);
        }
        return refState.at(ref).at(index);
    }

    record::ClassType fineClassifierECigar(const std::string& ref, const std::string& seq,
                                                              const std::string& ecigar);

    record::ClassType fineClassifierRecord(ReferenceManager::ReferenceExcerpt& record_reference,
                                                              const core::record::Record& rec, bool loadonly);

    /**
     *
     * @param _auSize
     */
    ClassifierRegroup(size_t _auSize, ReferenceManager* rfmgr);

    /**
     *
     * @return
     */
    record::Chunk getChunk() override;

    /**
     *
     * @param c
     */
    void add(record::Chunk&& c) override;

    /**
     *
     */
    void flush() override;

    /**
     *
     * @param cigar
     * @return
     */
    static record::ClassType classifyECigar(const std::string& cigar);

    /**
     *
     * @param seq
     * @return
     */
    static record::ClassType classifySeq(const std::string& seq);

    /**
     *
     * @param r
     * @return
     */
    static record::ClassType coarseClassify(const record::Record& r);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CLASSIFIERREGROUP_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
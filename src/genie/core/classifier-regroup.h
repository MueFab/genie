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

    size_t auSize;                                  //!<
    core::stats::PerfStats stats;

    bool isCovered(size_t start, size_t end) const{
        size_t position = start;

        for(auto it = currentSeqCoverage.begin(); it != currentSeqCoverage.end(); ++it) {
            if(position >= (end - 1)) {
                return true;
            }
            if(position >= it->first && position < it->second) {
                position = it->second - 1;
                it = currentSeqCoverage.begin();
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



   public:
    /**
     *
     * @param _auSize
     */
    explicit ClassifierRegroup(size_t _auSize);

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
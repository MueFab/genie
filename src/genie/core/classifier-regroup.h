/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_REGROUP_H_
#define SRC_GENIE_CORE_CLASSIFIER_REGROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/classifier.h"
#include "genie/core/record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ClassifierRegroup : public Classifier {
 public:
    enum class RefMode { NONE = 0, RELEVANT = 1, FULL = 2 };

 private:
    std::vector<record::Chunk> finishedChunks;  //!<

    using ClassBlock = std::vector<record::Chunk>;  //!<
    using PairedBlock = std::vector<ClassBlock>;    //!<
    using RefNoRefBlock = std::vector<PairedBlock>;
    RefNoRefBlock currentChunks;                                //!<
    ReferenceManager* refMgr;                                   //!<
    int16_t currentSeqID;                                       //!<
    std::vector<std::pair<size_t, size_t>> currentSeqCoverage;  //!<
    std::map<std::string, std::vector<uint8_t>> refState;       //!<

    size_t auSize;                 //!<
    core::stats::PerfStats stats;  //!<
    RefMode refMode;               //!<
    size_t refModeFullSeqID{0};    //!<
    size_t refModeFullCovID{0};    //!<
    size_t refModeFullChunkID{0};  //!<

    bool rawRefMode = true;  //!<

    /**
     *
     * @param start
     * @param end
     * @return
     */
    bool isCovered(size_t start, size_t end) const;

    /**
     *
     * @param r
     * @return
     */
    bool isCovered(const core::record::Record& r) const;

    /**
     *
     * @param refBased
     * @param paired
     * @param classtype
     * @param r
     */
    void push(bool refBased, bool paired, core::record::ClassType classtype, core::record::Record& r);

    /**
     *
     * @param data
     */
    void queueFinishedChunk(core::record::Chunk& data);

 public:
    /**
     *
     * @param ref
     * @param index
     * @return
     */
    bool isWritten(const std::string& ref, size_t index);

    /**
     *
     * @param ref
     * @param seq
     * @param ecigar
     * @return
     */
    record::ClassType fineClassifierECigar(const std::string& ref, const std::string& seq, const std::string& ecigar);

    /**
     *
     * @param record_reference
     * @param rec
     * @param loadonly
     * @return
     */
    record::ClassType fineClassifierRecord(ReferenceManager::ReferenceExcerpt& record_reference,
                                           const core::record::Record& rec, bool loadonly);

    /**
     *
     * @param _auSize
     * @param rfmgr
     * @param mode
     * @param raw_ref
     */
    ClassifierRegroup(size_t _auSize, ReferenceManager* rfmgr, RefMode mode, bool raw_ref);

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

#endif  // SRC_GENIE_CORE_CLASSIFIER_REGROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

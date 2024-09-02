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

namespace genie::core {

/**
 * @brief
 */
class ClassifierRegroup : public Classifier {
 public:
    enum class RefMode { NONE = 0, RELEVANT = 1, FULL = 2 };

 private:
    std::vector<record::Chunk> finishedChunks;  //!< @brief

    using ClassBlock = std::vector<record::Chunk>;  //!< @brief
    using PairedBlock = std::vector<ClassBlock>;    //!< @brief
    using RefNoRefBlock = std::vector<PairedBlock>;
    RefNoRefBlock currentChunks;                                //!< @brief
    record::Chunk current_unpaired_u_Chunk;                     //! @brief
    ReferenceManager* refMgr;                                   //!< @brief
    int16_t currentSeqID;                                       //!< @brief
    std::vector<std::pair<size_t, size_t>> currentSeqCoverage;  //!< @brief
    std::map<std::string, std::vector<uint8_t>> refState;       //!< @brief

    size_t auSize;                 //!< @brief
    core::stats::PerfStats stats;  //!< @brief
    RefMode refMode;               //!< @brief
    size_t refModeFullSeqID{0};    //!< @brief
    size_t refModeFullCovID{0};    //!< @brief
    size_t refModeFullChunkID{0};  //!< @brief

    bool rawRefMode = true;  //!< @brief

    /**
     * @brief
     * @param start
     * @param end
     * @return
     */
    [[nodiscard]] bool isCovered(size_t start, size_t end) const;

    /**
     * @brief
     * @param r
     * @return
     */
    [[nodiscard]] bool isCovered(const core::record::Record& r) const;

    /**
     * @brief
     * @param data
     */
    void queueFinishedChunk(core::record::Chunk& data);

 public:
    /**
     * @brief
     * @param ref
     * @param index
     * @return
     */
    bool isWritten(const std::string& ref, size_t index);

    /**
     * @brief
     * @param _auSize
     * @param rfmgr
     * @param mode
     * @param raw_ref
     */
    ClassifierRegroup(size_t _auSize, ReferenceManager* rfmgr, RefMode mode, bool raw_ref);

    /**
     * @brief
     * @return
     */
    record::Chunk getChunk() override;

    /**
     * @brief
     * @param c
     */
    void add(record::Chunk&& c) override;

    /**
     * @brief
     */
    void flush() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CLASSIFIER_REGROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

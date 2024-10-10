/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_RECORD_PILEUP_H_
#define SRC_GENIE_QUALITY_CALQ_RECORD_PILEUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/record/record.h"
#include "genie/quality/calq/calq_coder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief
 */
class RecordPileup {
    std::vector<EncodingRecord> records;
    std::vector<std::vector<std::string>> preprocessed_qvalues;
    std::vector<std::vector<std::string>> preprocessed_sequences;
    uint64_t minPos;
    uint64_t maxPos;

 public:
    /**
     * @brief
     */
    RecordPileup();

    /**
     *
     * @param pos
     * @return
     */
    std::pair<std::string, std::string> getPileup(uint64_t pos);

    /**
     * @brief
     * @param s
     */
    void addRecord(EncodingRecord &r);

    /**
     *
     * @return
     */
    [[nodiscard]] uint64_t getMinPos() const;

    /**
     *
     * @return
     */
    [[nodiscard]] uint64_t getMaxPos() const;

    /**
     *
     * @return
     */
    bool empty();

    /**
     *
     * @param positions
     * @param seqs
     * @param pos
     * @return
     */
    static bool isRecordBeforePos(const std::vector<uint64_t> &positions, const std::vector<std::string> &seqs,
                                  uint64_t pos);

    /**
     *
     * @param pos
     * @return
     */
    std::vector<EncodingRecord> getRecordsBefore(uint64_t pos);

    /**
     *
     * @return
     */
    std::vector<EncodingRecord> getAllRecords();

 private:
    /**
     * @brief
     */
    void nextRecord();

    /**
     * @brief
     * @param read
     * @param cigar
     * @return
     */
    static std::string preprocess(const std::string &read, const std::string &cigar);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::calq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_RECORD_PILEUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

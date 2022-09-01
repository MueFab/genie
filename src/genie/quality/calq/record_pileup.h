/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_Record_Pileup_H_
#define SRC_GENIE_QUALITY_CALQ_Record_Pileup_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include <vector>
#include "genie/core/record/record.h"
#include "calq_coder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

/**
 * @brief
 */
class RecordPileup {
    std::vector<::calq::EncodingRecord> records;
    std::vector<std::vector<std::string>> preprocessed_qvalues;
    std::vector<std::vector<std::string>> preprocessed_sequences;
    uint64_t minPos;
    uint64_t maxPos;

 public:
    /**
     * @brief
     */
    explicit RecordPileup();

    /**
     *
     * @param pos
     * @return
     */
    std::pair<std::string, std::string> getPileup(uint64_t pos);

    /**
     *
     * @param sequence
     * @param qualities
     * @param ecigar
     * @param position
     */
//    void addSingleRead(const std::string &sequence, const std::string &qualities, const std::string &ecigar,
//                       uint64_t position);

    /**
     * @brief
     * @param s
     */
    void addRecord(::calq::EncodingRecord &r);

    /**
     *
     * @return
     */
    uint64_t getMinPos() const;

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
    std::tuple<std::vector<std::vector<uint64_t>>, std::vector<std::vector<std::string>>,
               std::vector<std::vector<std::string>>>
    getRecordsBefore(uint64_t pos);

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

    /**
     * @brief
     * @param cigar
     * @return
     */
    static uint32_t lengthFromCigar(const std::string &cigar);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  //SRC_GENIE_QUALITY_CALQ_Record_Pileup_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

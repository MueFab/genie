/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_LOCAL_REFERENCE_H_
#define SRC_GENIE_QUALITY_CALQ_LOCAL_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include <vector>
#include "genie/core/record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

/**
 * @brief
 */
class LocalReference {
    std::vector<std::vector<std::string>> sequences;        //!< @brief
    std::vector<std::vector<uint64_t>> sequence_positions;  //!< @brief
    std::vector<std::vector<std::string>> qualities;
    uint64_t minPos;
    uint64_t maxPos;

 public:
    /**
     * @brief
     */
    explicit LocalReference();

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
    void addSingleRead(const std::string &sequence, const std::string &qualities, const std::string &ecigar,
                       uint64_t position);

    /**
     * @brief
     * @param s
     */
    void addRead(const core::record::Record &s);

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

#endif  // SRC_GENIE_QUALITY_CALQ_LOCAL_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

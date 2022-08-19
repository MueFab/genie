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
    uint32_t cr_buf_max_size;  //!< @brief

    std::vector<std::vector<std::string>> sequences;        //!< @brief
    std::vector<std::vector<uint64_t>> sequence_positions;  //!< @brief
    std::vector<std::vector<std::string>> qualities;
    uint32_t crBufSize;  //!< @brief
    uint64_t minPos;
    uint64_t maxPos;

 public:
    /**
     * @brief
     * @param offset
     * @param len
     * @return
     */
    //    std::string generateRef(uint32_t offset, uint32_t len);

    /**
     * @brief
     * @param offset_to_first
     * @return
     */
    //    char majorityVote(uint32_t offset_to_first);

    uint64_t getMinPos() const;

    bool empty();

    static bool isRecordBeforePos(const std::vector<uint64_t>& positions, const std::vector<std::string>& seqs, uint64_t pos);

    std::tuple<std::vector<std::vector<uint64_t>>, std::vector<std::vector<std::string>>,
               std::vector<std::vector<std::string>>>
    getRecordsBefore(uint64_t pos);

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

 public:
    /**
     * @brief
     * @param _cr_buf_max_size
     */
    explicit LocalReference(uint32_t _cr_buf_max_size);

    /**
     * @brief
     * @param record
     * @param ecigar
     * @param position
     */
    //    void addSingleRead(const std::string &record, const std::string &ecigar, uint64_t position);

    std::pair<std::string, std::string> getPileup(uint64_t pos);

    void addSingleRead(const std::string &sequence, const std::string &qualities, const std::string &ecigar,
                       uint64_t position);

    /**
     * @brief
     * @param s
     */
    void addRead(const core::record::Record &s);

    /**
     * @brief
     * @param pos_offset
     * @param cigar
     * @return
     */
    //    std::string getReference(uint32_t pos_offset, const std::string &cigar);

    /**
     * @brief
     * @param pos_offset
     * @param len
     * @return
     */
    //    std::string getReference(uint32_t pos_offset, uint32_t len);

    /**
     * @brief
     * @return
     */
    //    uint64_t getWindowBorder() const;

    /**
     * @brief
     */
    //    void printWindow() const;

    /**
     * @brief
     * @return
     */
    uint32_t getMaxBufferSize() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_LOCAL_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

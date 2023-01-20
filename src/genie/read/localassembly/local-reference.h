/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_LOCAL_REFERENCE_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_LOCAL_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include <vector>
#include "genie/core/record/alignment/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

/**
 * @brief
 */
class LocalReference {
 public:
    uint32_t cr_buf_max_size;  //!< @brief

    std::vector<std::string> sequences;        //!< @brief
    std::vector<uint64_t> sequence_positions;  //!< @brief
    uint32_t crBufSize;                        //!< @brief

    /**
     * @brief
     * @param offset
     * @param len
     * @return
     */
    std::string generateRef(uint32_t offset, uint32_t len);

    /**
     * @brief
     * @param offset_to_first
     * @return
     */
    char majorityVote(uint32_t offset_to_first);

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
    void addSingleRead(const std::string &record, const std::string &ecigar, uint64_t position);

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
    std::string getReference(uint32_t pos_offset, const std::string &cigar);

    /**
     * @brief
     * @param pos_offset
     * @param len
     * @return
     */
    std::string getReference(uint32_t pos_offset, uint32_t len);

    /**
     * @brief
     * @return
     */
    uint64_t getWindowBorder() const;

    /**
     * @brief
     */
    void printWindow() const;

    /**
     * @brief
     * @return
     */
    uint32_t getMaxBufferSize() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_LOCAL_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

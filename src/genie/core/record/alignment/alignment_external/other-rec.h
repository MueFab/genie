/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_OTHER_REC_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_OTHER_REC_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include "genie/core/record/alignment/alignment-external.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_external {

/**
 * @brief
 */
class OtherRec : public AlignmentExternal {
    uint64_t next_pos;     //!< @brief
    uint16_t next_seq_ID;  //!< @brief

 public:
    /**
     * @brief
     * @param _next_pos
     * @param _next_seq_ID
     */
    OtherRec(uint64_t _next_pos, uint16_t _next_seq_ID);

    /**
     * @brief
     * @return
     */
    uint64_t getNextPos() const;

    /**
     * @brief
     * @return
     */
    uint16_t getNextSeq() const;

    /**
     * @brief
     */
    OtherRec();

    /**
     * @brief
     * @param reader
     */
    explicit OtherRec(util::BitReader &reader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @return
     */
    std::unique_ptr<AlignmentExternal> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_external
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_OTHER_REC_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

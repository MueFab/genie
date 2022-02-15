/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_OTHER_REC_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_OTHER_REC_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include "genie/core/record/alignment-split.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_split {

/**
 * @brief
 */
class OtherRec : public AlignmentSplit {
 private:
    uint64_t split_pos;     //!< @brief
    uint16_t split_seq_ID;  //!< @brief
    uint8_t reverse_comp;                //!< @brief
    std::string ecigar_string;           //!< @brief

 public:
    /**
     * @brief
     */
    OtherRec();

    /**
     * @brief
     * @param _split_pos
     * @param _split_seq_ID
     */
    OtherRec(uint64_t _split_pos, uint16_t _split_seq_ID, uint8_t _reverse_comp = 0, std::string &&_ecigar_string = "");

    /**
     * @brief
     * @param reader
     */
    explicit OtherRec(bool extended_alignment_info, util::BitReader& reader);

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
     * @return
     */
    uint8_t getRComp() const;

    /**
     * @brief
     * @return
     */
    const std::string& getECigar() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    std::unique_ptr<AlignmentSplit> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_split
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_OTHER_REC_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

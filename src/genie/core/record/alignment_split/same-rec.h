/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_SAME_REC_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_SAME_REC_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include "genie/core/record/alignment-split.h"
#include "genie/core/record/alignment.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

/**
 * @brief
 */
class SameRec : public AlignmentSplit {
 private:
    int64_t delta;        //!< @brief
    Alignment alignment;  //!< @brief

 public:
    /**
     * @brief
     */
    SameRec();

    /**
     * @brief
     * @param _delta
     * @param _alignment
     */
    SameRec(int64_t _delta, Alignment _alignment);

    /**
     * @brief
     * @param as_depth
     * @param reader
     */
    explicit SameRec(uint8_t as_depth, util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const Alignment& getAlignment() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] int64_t getDelta() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::unique_ptr<AlignmentSplit> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_SAME_REC_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_SAME_REC_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_SAME_REC_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>

#include "genie/core/record/alignment.h"
#include "genie/core/record/alignment_split.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

/**
 * @brief
 */
class SameRec final : public AlignmentSplit {
 private:
    int64_t delta_;        //!< @brief
    Alignment alignment_;  //!< @brief

 public:
    /**
     * @brief
     */
    SameRec();

    /**
     * @brief
     * @param delta
     * @param alignment
     */
    SameRec(int64_t delta, Alignment alignment);

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
    void Write(util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const Alignment& GetAlignment() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]]  int64_t getDelta() const;

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

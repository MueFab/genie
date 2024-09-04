/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_UNPAIRED_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_UNPAIRED_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/record/alignment-split.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

/**
 * @brief
 */
class Unpaired : public AlignmentSplit {
 public:
    /**
     * @brief
     */
    Unpaired();

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::unique_ptr<AlignmentSplit> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_UNPAIRED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

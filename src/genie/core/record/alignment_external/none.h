/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_NONE_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/record/alignment-external.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

/**
 * @brief
 */
class None : public AlignmentExternal {
 public:
    /**
     * @brief
     */
    None();

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::unique_ptr<AlignmentExternal> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_external

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

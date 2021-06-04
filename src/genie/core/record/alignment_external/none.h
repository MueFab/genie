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

namespace genie {
namespace core {
namespace record {
namespace alignment_external {

/**
 *
 */
class None : public AlignmentExternal {
 public:
    /**
     *
     */
    None();

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     *
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

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

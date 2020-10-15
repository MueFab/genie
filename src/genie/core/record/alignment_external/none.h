/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ALIGNMENT_EXT_NONE_H
#define GENIE_ALIGNMENT_EXT_NONE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <record/alignment-external.h>

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

#endif  // GENIE_NONE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
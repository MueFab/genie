/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_RAW_REFERENCE_H
#define GENIE_RAW_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/data_unit.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <vector>
#include "raw_reference_seq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class RawReference : public core::parameter::DataUnit {
   private:
    std::vector<RawReferenceSequence> seqs;  //!<

   public:
    /**
     *
     */
    RawReference();

    /**
     *
     */
    ~RawReference() override = default;

    /**
     *
     * @param ref
     */
    void addSequence(RawReferenceSequence&& ref);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RAW_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

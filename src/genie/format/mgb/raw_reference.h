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
 * ISO 23092-2 Section 3.2 table 5
 */
class RawReference : public core::parameter::DataUnit {
   private:
    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
     * -------------------------------------------------------------------------------------------------------------- */

    //!< uint64_t data_unit_size : 64; //!< Line 4, currently computed on the fly

    /** ------------------------------------------------------------------------------------------------------------- */

    //!< uint16_t seq_count : 16; //!< Line 2, currently computed by vector
    std::vector<std::unique_ptr<RawReferenceSequence>> seqs;  //!< For loop Lines 3 to 8

   public:
    RawReference();

    virtual ~RawReference() = default;

    void addSequence(std::unique_ptr<RawReferenceSequence> ref);

    std::unique_ptr<RawReference> clone() const;

    void write(util::BitWriter &writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RAW_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
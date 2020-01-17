/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_COMPUTED_REF_H
#define GENIE_COMPUTED_REF_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/make-unique.h>
#include <cstdint>
#include <memory>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include "computed_ref_extended.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

/**
 * ISO 23092-2 Section 3.3.2.3 table 16
 */
class ComputedRef {
   public:
    /**
     * ISO 23092-2 Section 3.3.2.3 table 17
     */
    enum class Algorithm : uint8_t {
        RESERVED = 0,
        REF_TRANSFORM = 1,
        PUSH_IN = 2,
        LOCAL_ASSEMBLY = 3,
        GLOBAL_ASSEMBLY = 4
    };

   private:
    Algorithm cr_alg_ID;                             //!<< : 8; Line 2
    std::unique_ptr<ComputedRefExtended> extension;  //!<< Lines 3 to 6

   public:
    explicit ComputedRef(Algorithm _cr_alg_ID);

    explicit ComputedRef(util::BitReader &reader);

    virtual ~ComputedRef() = default;

    void setExtension(std::unique_ptr<ComputedRefExtended> _crps_info);

    const ComputedRefExtended &getExtension() const;

    virtual void write(util::BitWriter &bw) const;

    std::unique_ptr<ComputedRef> clone() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_COMPUTED_REF_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
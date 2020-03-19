/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_STATE_VARS_H
#define GENIE_STATE_VARS_H

// ---------------------------------------------------------------------------------------------------------------------

#include "binarization.h"
#include "support_values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.2 table
 */
class StateVars {
   private:
    uint64_t numAlphaSubsym;
    uint32_t numSubsyms;
    uint32_t numCtxSubsym;
    uint32_t cLengthBI;
    uint32_t codingOrderCtxOffset[3];
    uint32_t codingSizeCtxOffset;
    uint32_t numCtxLuts;
    uint32_t numCtxTotal;

   public:

    static const uint32_t MAX_LUT_SIZE = (1u<<8u);

    StateVars();

    void populate(const SupportValues::TransformIdSubsym transform_ID_subsym,
                  const SupportValues support_values,
                  const Binarization cabac_binarization);

    virtual ~StateVars() = default;

    uint32_t getNumSubsymbols() const {
        return numSubsyms;
    }

    uint32_t getNumCtxPerSubsymbol() const {
        return numCtxSubsym;
    }

    uint32_t getCLengthBI() const {
        return cLengthBI;
    }

    uint32_t getNumCtxForLUTs() const {
        return numCtxLuts;
    }

    uint32_t getNumCtxTotal() const {
        return numCtxTotal;
    }

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_STATE_VARS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

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

    uint32_t getCodingOrderCtxOffset(uint8_t index) const {
        return codingOrderCtxOffset[index];
    }

    uint32_t getCodingSizeCtxOffset() const {
        return codingSizeCtxOffset;
    }

    uint32_t getNumCtxLUTs() const {
        return numCtxLuts;
    }

    uint32_t getNumCtxTotal() const {
        return numCtxTotal;
    }

    /* some extra functions (for now put in statevars)
     * FIXME move else where
     */
    static
    uint64_t get2PowN(uint8_t N) {
        assert(N<=32);
        uint64_t one = 1u;
        return one<<N;
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

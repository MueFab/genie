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

#include <genie/core/constants.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.2 table
 */
class StateVars {
   private:
    uint64_t numAlphaSubsym;           //!<
    uint32_t numSubsyms;               //!<
    uint32_t numCtxSubsym;             //!<
    uint32_t cLengthBI;                //!<
    uint64_t codingOrderCtxOffset[3];  //!<
    uint64_t codingSizeCtxOffset;      //!<
    uint32_t numCtxLuts;               //!<
    uint64_t numCtxTotal;              //!<

   public:
    static const uint32_t MAX_LUT_SIZE = (1u << 8u);  //!<

    /**
     *
     */
    StateVars();

    /**
     *
     * @param transform_ID_subsym
     * @param support_values
     * @param cabac_binarization
     * @param subsequence_ID
     * @param alphabet_ID
     */
    void populate(const SupportValues::TransformIdSubsym transform_ID_subsym, const SupportValues support_values,
                  const Binarization cabac_binarization, const core::GenSubIndex subsequence_ID,
                  const core::AlphabetID alphabet_ID);

    /**
     *
     */
    virtual ~StateVars() = default;

    /**
     *
     * @return
     */
    uint32_t getNumSubsymbols() const;

    /**
     *
     * @return
     */
    uint64_t getNumAlphaSubsymbol() const;

    /**
     *
     * @return
     */
    uint32_t getNumCtxPerSubsymbol() const;

    /**
     *
     * @return
     */
    uint32_t getCLengthBI() const;

    /**
     *
     * @param index
     * @return
     */
    uint64_t getCodingOrderCtxOffset(uint8_t index) const;

    /**
     *
     * @return
     */
    uint64_t getCodingSizeCtxOffset() const;

    /**
     *
     * @return
     */
    uint32_t getNumCtxLUTs() const;

    /**
     *
     * @return
     */
    uint64_t getNumCtxTotal() const;

    /**
     *
     * @param codingOrder
     * @param shareSubsymLutFlag
     * @param trnsfSubsymID
     * @return
     */
    uint8_t getNumLuts(const uint8_t codingOrder, const bool shareSubsymLutFlag,
                       const SupportValues::TransformIdSubsym trnsfSubsymID) const;

    /**
     *
     * @param shareSubsymPrvFlag
     * @return
     */
    uint8_t getNumPrvs(const bool shareSubsymPrvFlag) const;

    /* some extra functions (for now put in statevars)
     * FIXME move else where
     */

    /**
     *
     * @param subsequence_ID
     * @param alphabet_ID
     * @return
     */
    static uint64_t getNumAlphaSpecial(const core::GenSubIndex subsequence_ID, const core::AlphabetID alphabet_ID);

    /**
     *
     * @param sizeInBits
     * @return
     */
    static uint8_t getMinimalSizeInBytes(uint8_t sizeInBits);

    /**
     *
     * @param sizeInBits
     * @return
     */
    static uint8_t getLgWordSize(uint8_t sizeInBits);

    /**
     *
     * @param input
     * @param sizeInBytes
     * @return
     */
    static int64_t getSignedValue(uint64_t input, uint8_t sizeInBytes);

    /**
     *
     * @param N
     * @return
     */
    static uint64_t get2PowN(uint8_t N);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_STATE_VARS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

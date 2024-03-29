/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/constants.h"
#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/support_values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * @brief
 */
class StateVars {
 private:
    uint64_t numAlphaSubsym;           //!< @brief
    uint32_t numSubsyms;               //!< @brief
    uint32_t numCtxSubsym;             //!< @brief
    uint32_t cLengthBI;                //!< @brief
    uint64_t codingOrderCtxOffset[3];  //!< @brief
    uint64_t codingSizeCtxOffset;      //!< @brief
    uint32_t numCtxLuts;               //!< @brief
    uint64_t numCtxTotal;              //!< @brief

 public:
    static const uint32_t MAX_LUT_SIZE = (1u << 8u);  //!< @brief

    /**
     * @brief
     * @param bin
     * @return
     */
    bool operator==(const StateVars& bin) const;

    /**
     * @brief
     */
    StateVars();

    /**
     * @brief
     * @param transform_ID_subsym
     * @param support_values
     * @param cabac_binarization
     * @param subsequence_ID
     * @param alphabet_ID
     * @param original
     */
    void populate(const SupportValues::TransformIdSubsym transform_ID_subsym, const SupportValues support_values,
                  const Binarization cabac_binarization, const core::GenSubIndex subsequence_ID,
                  const core::AlphabetID alphabet_ID, bool original = true);

    /**
     * @brief
     */
    virtual ~StateVars() = default;

    /**
     * @brief
     * @return
     */
    uint32_t getNumSubsymbols() const;

    /**
     * @brief
     * @return
     */
    uint64_t getNumAlphaSubsymbol() const;

    /**
     * @brief
     * @return
     */
    uint32_t getNumCtxPerSubsymbol() const;

    /**
     * @brief
     * @return
     */
    uint32_t getCLengthBI() const;

    /**
     * @brief
     * @param index
     * @return
     */
    uint64_t getCodingOrderCtxOffset(uint8_t index) const;

    /**
     * @brief
     * @return
     */
    uint64_t getCodingSizeCtxOffset() const;

    /**
     * @brief
     * @return
     */
    uint32_t getNumCtxLUTs() const;

    /**
     * @brief
     * @return
     */
    uint64_t getNumCtxTotal() const;

    /**
     * @brief
     * @param codingOrder
     * @param shareSubsymLutFlag
     * @param trnsfSubsymID
     * @return
     */
    uint8_t getNumLuts(const uint8_t codingOrder, const bool shareSubsymLutFlag,
                       const SupportValues::TransformIdSubsym trnsfSubsymID) const;

    /**
     * @brief
     * @param shareSubsymPrvFlag
     * @return
     */
    uint8_t getNumPrvs(const bool shareSubsymPrvFlag) const;

    /* some extra functions (for now put in statevars)
     * FIXME move else where
     */

    /**
     * @brief
     * @param subsequence_ID
     * @param alphabet_ID
     * @return
     */
    static uint64_t getNumAlphaSpecial(const core::GenSubIndex subsequence_ID, const core::AlphabetID alphabet_ID);

    /**
     * @brief
     * @param sizeInBits
     * @return
     */
    static uint8_t getMinimalSizeInBytes(uint8_t sizeInBits);

    /**
     * @brief
     * @param sizeInBits
     * @return
     */
    static uint8_t getLgWordSize(uint8_t sizeInBits);

    /**
     * @brief
     * @param input
     * @param sizeInBytes
     * @return
     */
    static int64_t getSignedValue(uint64_t input, uint8_t sizeInBytes);

    /**
     * @brief
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

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

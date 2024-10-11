/**
 * @file
 * @brief Defines the `StateVars` class, which manages various state variables for CABAC entropy coding.
 * @details This class provides a structure to hold and compute necessary parameters and variables for CABAC
 *          (Context-based Adaptive Binary Arithmetic Coding). It supports different transformation types and encodes
 *          contexts for subsequence transformations. The `StateVars` class handles state variables such as context
 *          offsets, number of contexts, and number of Look-Up Tables (LUTs) used in entropy coding.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/constants.h"
#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/support_values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Manages state variables used for entropy coding in CABAC transformations.
 * @details This class computes and stores various parameters required during CABAC encoding and decoding. It is
 *          responsible for defining the structure of contexts, offsets, and other essential state variables that
 *          govern the coding process for transformed subsequences.
 */
class StateVars {
 private:
    uint64_t numAlphaSubsym;           //!< @brief Number of alphabet symbols per subsequence.
    uint32_t numSubsyms;               //!< @brief Number of subsequence symbols.
    uint32_t numCtxSubsym;             //!< @brief Number of contexts per subsequence symbol.
    uint32_t cLengthBI;                //!< @brief Length of context variables for binary interval calculations.
    uint64_t codingOrderCtxOffset[3];  //!< @brief Offset array for coding order contexts.
    uint64_t codingSizeCtxOffset;      //!< @brief Offset value for coding size contexts.
    uint32_t numCtxLuts;               //!< @brief Number of context Look-Up Tables (LUTs).
    uint64_t numCtxTotal;              //!< @brief Total number of contexts.

 public:
    static const uint32_t MAX_LUT_SIZE = (1u << 8u);  //!< @brief Maximum size of the LUT.

    /**
     * @brief Equality operator for comparing two `StateVars` objects.
     * @param bin The other `StateVars` object to compare with.
     * @return True if both objects are equal, otherwise false.
     */
    bool operator==(const StateVars& bin) const;

    /**
     * @brief Default constructor. Initializes default values for all state variables.
     */
    StateVars();

    /**
     * @brief Populates the state variables based on the transformation type and configuration parameters.
     * @param transform_ID_subsym The type of transformation used on subsequence symbols.
     * @param support_values The support values used for CABAC coding.
     * @param cabac_binarization The binarization scheme used for CABAC coding.
     * @param subsequence_ID ID of the subsequence being encoded or decoded.
     * @param alphabet_ID ID of the alphabet used in this subsequence.
     * @param original Boolean flag indicating whether this is the original sequence.
     */
    void populate(SupportValues::TransformIdSubsym transform_ID_subsym, const SupportValues& support_values,
                  const Binarization& cabac_binarization, core::GenSubIndex subsequence_ID,
                  core::AlphabetID alphabet_ID, bool original = true);

    /**
     * @brief Virtual destructor.
     */
    virtual ~StateVars() = default;

    /**
     * @brief Retrieves the number of subsequence symbols.
     * @return The number of subsequence symbols.
     */
    [[nodiscard]] uint32_t getNumSubsymbols() const;

    /**
     * @brief Retrieves the number of alphabet symbols per subsequence.
     * @return The number of alphabet symbols.
     */
    [[nodiscard]] uint64_t getNumAlphaSubsymbol() const;

    /**
     * @brief Retrieves the number of contexts per subsequence symbol.
     * @return The number of contexts per subsequence symbol.
     */
    [[nodiscard]] uint32_t getNumCtxPerSubsymbol() const;

    /**
     * @brief Retrieves the binary interval length.
     * @return The binary interval length used for context coding.
     */
    [[nodiscard]] uint32_t getCLengthBI() const;

    /**
     * @brief Retrieves the coding order context offset at a specific index.
     * @param index Index of the offset value to retrieve (range 0-2).
     * @return The coding order context offset at the specified index.
     */
    [[nodiscard]] uint64_t getCodingOrderCtxOffset(uint8_t index) const;

    /**
     * @brief Retrieves the coding size context offset.
     * @return The coding size context offset value.
     */
    [[nodiscard]] uint64_t getCodingSizeCtxOffset() const;

    /**
     * @brief Retrieves the number of context Look-Up Tables (LUTs).
     * @return The number of context LUTs.
     */
    [[nodiscard]] uint32_t getNumCtxLUTs() const;

    /**
     * @brief Retrieves the total number of contexts.
     * @return The total number of contexts.
     */
    [[nodiscard]] uint64_t getNumCtxTotal() const;

    /**
     * @brief Computes the number of LUTs based on coding order and sharing flags.
     * @param codingOrder The coding order of the subsequence.
     * @param shareSubsymLutFlag Boolean flag indicating if subsequence LUTs are shared.
     * @param trnsfSubsymID ID of the transformation type applied to subsequence symbols.
     * @return The computed number of LUTs.
     */
    [[nodiscard]] uint8_t getNumLuts(uint8_t codingOrder, bool shareSubsymLutFlag,
                                     SupportValues::TransformIdSubsym trnsfSubsymID) const;

    /**
     * @brief Computes the number of prediction variables based on sharing flag.
     * @param shareSubsymPrvFlag Boolean flag indicating if previous contexts are shared.
     * @return The computed number of prediction variables.
     */
    [[nodiscard]] uint8_t getNumPrvs(bool shareSubsymPrvFlag) const;

    // Static utility functions for additional computations

    /**
     * @brief Computes the number of alphabet symbols for special subsequence types.
     * @param subsequence_ID ID of the subsequence.
     * @param alphabet_ID ID of the alphabet used in the subsequence.
     * @return The number of alphabet symbols for special subsequences.
     */
    static uint64_t getNumAlphaSpecial(core::GenSubIndex subsequence_ID, core::AlphabetID alphabet_ID);

    /**
     * @brief Calculates the minimal byte size needed for a given number of bits.
     * @param sizeInBits The number of bits.
     * @return The minimal number of bytes required.
     */
    static uint8_t getMinimalSizeInBytes(uint8_t sizeInBits);

    /**
     * @brief Computes the logarithm of word size.
     * @param sizeInBits The number of bits.
     * @return The logarithm of the word size.
     */
    static uint8_t getLgWordSize(uint8_t sizeInBits);

    /**
     * @brief Converts a signed integer value.
     * @param input The input unsigned integer value.
     * @param sizeInBytes The byte size of the value.
     * @return The signed integer representation.
     */
    static int64_t getSignedValue(uint64_t input, uint8_t sizeInBytes);

    /**
     * @brief Computes 2 raised to the power of `N`.
     * @param N The exponent value.
     * @return The result of 2^N.
     */
    static uint64_t get2PowN(uint8_t N);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

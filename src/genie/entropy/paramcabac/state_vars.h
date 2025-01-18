/**
 * Copyright 2018-2024 The Genie Authors.
 * @file state_vars.h
 * @brief Defines the `StateVars` class, which manages various state variables
 * for CABAC entropy coding.
 * @details This class provides a structure to hold and compute necessary
 * parameters and variables for CABAC (Context-based Adaptive Binary Arithmetic
 * Coding). It supports different transformation types and encodes contexts for
 * subsequence transformations. The `StateVars` class handles state variables
 * such as context offsets, number of contexts, and number of Look-Up Tables
 * (LUTs) used in entropy coding.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_

// -----------------------------------------------------------------------------

#include "genie/core/constants.h"
#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/support_values.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

constexpr uint32_t kMaxLutSize = 1u << 8u;

/**
 * @brief Manages state variables used for entropy coding in CABAC
 * transformations.
 * @details This class computes and stores various parameters required during
 * CABAC encoding and decoding. It is responsible for defining the structure of
 * contexts, offsets, and other essential state variables that govern the coding
 * process for transformed subsequences.
 */
class StateVars final {
  uint64_t num_alpha_subsym_;  //!< @brief Number of alphabet symbols per
                               //!< subsequence.
  uint32_t num_subsyms_;       //!< @brief Number of subsequence symbols.
  uint32_t
      num_ctx_subsym_;    //!< @brief Number of contexts per subsequence symbol.
  uint32_t c_length_bi_;  //!< @brief Length of context variables for binary
                          //!< interval calculations.
  uint64_t coding_order_ctx_offset_[3];  //!< @brief Offset array for coding
                                         //!< order contexts.
  uint64_t coding_size_ctx_offset_;  //!< @brief Offset value for coding Size
                                     //!< contexts.
  uint32_t num_ctx_luts_;   //!< @brief Number of context Look-Up Tables (LUTs).
  uint64_t num_ctx_total_;  //!< @brief Total number of contexts.

 public:
  /**
   * @brief Equality operator for comparing two `StateVars` objects.
   * @param bin The other `StateVars` object to compare with.
   * @return True if both objects are equal, otherwise false.
   */
  bool operator==(const StateVars& bin) const;

  /**
   * @brief Default constructor. Initializes default values for all state
   * variables.
   */
  StateVars();

  /**
   * @brief Populates the state variables based on the transformation type and
   * configuration parameters.
   * @param transform_id_subsym The type of transformation used on subsequence
   * symbols.
   * @param support_values The support values used for CABAC coding.
   * @param cabac_binarization The binarization scheme used for CABAC coding.
   * @param subsequence_id ID of the subsequence being encoded or decoded.
   * @param alphabet_id ID of the alphabet used in this subsequence.
   * @param original Boolean flag indicating whether this is the original
   * sequence.
   */
  void populate(SupportValues::TransformIdSubsym transform_id_subsym,
                const SupportValues& support_values,
                const Binarization& cabac_binarization,
                const core::GenSubIndex& subsequence_id,
                core::AlphabetId alphabet_id, bool original = true);

  /**
   * @brief Retrieves the number of subsequence symbols.
   * @return The number of subsequence symbols.
   */
  [[nodiscard]] uint32_t GetNumSubsymbols() const;

  /**
   * @brief Retrieves the number of alphabet symbols per subsequence.
   * @return The number of alphabet symbols.
   */
  [[nodiscard]] uint64_t GetNumAlphaSubsymbol() const;

  /**
   * @brief Retrieves the number of contexts per subsequence symbol.
   * @return The number of contexts per subsequence symbol.
   */
  [[nodiscard]] uint32_t GetNumCtxPerSubsymbol() const;

  /**
   * @brief Retrieves the binary interval length.
   * @return The binary interval length used for context coding.
   */
  [[nodiscard]] uint32_t GetCLengthBi() const;

  /**
   * @brief Retrieves the coding order context offset at a specific index.
   * @param index Index of the offset value to retrieve (range 0-2).
   * @return The coding order context offset at the specified index.
   */
  [[nodiscard]] uint64_t GetCodingOrderCtxOffset(uint8_t index) const;

  /**
   * @brief Retrieves the coding Size context offset.
   * @return The coding Size context offset value.
   */
  [[nodiscard]] uint64_t GetCodingSizeCtxOffset() const;

  /**
   * @brief Retrieves the number of context Look-Up Tables (LUTs).
   * @return The number of context LUTs.
   */
  [[nodiscard]] uint32_t GetNumCtxLuts() const;

  /**
   * @brief Retrieves the total number of contexts.
   * @return The total number of contexts.
   */
  [[nodiscard]] uint64_t GetNumCtxTotal() const;

  /**
   * @brief Computes the number of LUTs based on coding order and sharing
   * flags.
   * @param coding_order The coding order of the subsequence.
   * @param share_subsym_lut_flag Boolean flag indicating if subsequence LUTs
   * are shared.
   * @param trnsf_subsym_id ID of the transformation type applied to subsequence
   * symbols.
   * @return The computed number of LUTs.
   */
  [[nodiscard]] uint8_t GetNumLuts(
      uint8_t coding_order, bool share_subsym_lut_flag,
      SupportValues::TransformIdSubsym trnsf_subsym_id) const;

  /**
   * @brief Computes the number of prediction variables based on sharing flag.
   * @param share_subsym_prv_flag Boolean flag indicating if previous contexts
   * are shared.
   * @return The computed number of prediction variables.
   */
  [[nodiscard]] uint8_t GetNumPrvs(bool share_subsym_prv_flag) const;

  // Static utility functions for additional computations

  /**
   * @brief Computes the number of alphabet symbols for special subsequence
   * types.
   * @param subsequence_id ID of the subsequence.
   * @param alphabet_id ID of the alphabet used in the subsequence.
   * @return The number of alphabet symbols for special subsequences.
   */
  static uint64_t GetNumAlphaSpecial(const core::GenSubIndex& subsequence_id,
                                     core::AlphabetId alphabet_id);

  /**
   * @brief Calculates the minimal byte Size needed for a given number of
   * bits.
   * @param size_in_bits The number of bits.
   * @return The minimal number of bytes required.
   */
  static uint8_t GetMinimalSizeInBytes(uint8_t size_in_bits);

  /**
   * @brief Computes the logarithm of word Size.
   * @param size_in_bits The number of bits.
   * @return The logarithm of the word Size.
   */
  static uint8_t GetLgWordSize(uint8_t size_in_bits);

  /**
   * @brief Converts a signed integer value.
   * @param input The input unsigned integer value.
   * @param size_in_bytes The byte Size of the value.
   * @return The signed integer representation.
   */
  static int64_t GetSignedValue(uint64_t input, uint8_t size_in_bytes);

  /**
   * @brief Computes 2 raised to the power of `N`.
   * @param n The exponent value.
   * @return The result of 2^N.
   */
  static uint64_t Get2PowN(uint8_t n);
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_STATE_VARS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

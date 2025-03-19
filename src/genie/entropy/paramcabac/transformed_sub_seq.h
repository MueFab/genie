/**
 * Copyright 2018-2024 The Genie Authors.
 * @file transformed_sub_seq.h
 * @brief Defines the `TransformedSubSeq` class for managing CABAC-transformed
 * subsequences in entropy coding.
 * @details This class handles the properties and configurations for transformed
 * subsequences within a CABAC entropy coding framework. It provides support for
 * setting up different transforms, binarization, and state variable
 *          configurations, as well as for serializing and deserializing the
 * sequence properties to/from binary and JSON formats.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUB_SEQ_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUB_SEQ_H_

// -----------------------------------------------------------------------------

#include <nlohmann/json.hpp>

#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/state_vars.h"
#include "genie/entropy/paramcabac/support_values.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Represents a transformed subsequence used in CABAC entropy coding.
 * @details The `TransformedSubSeq` class manages configurations for
 * subsequences using CABAC (Context-based Adaptive Binary Arithmetic Coding)
 * transforms. It includes settings for transform ID, support values,
 * binarization configurations, and state variables. This class provides
 * functionality for serializing, deserializing, and converting the sequence
 * data to/from JSON.
 */
class TransformedSubSeq final {
 public:
  /**
   * @brief Default constructor. Initializes an empty transformed subsequence.
   */
  TransformedSubSeq();

  /**
   * @brief Parameterized constructor for creating a `TransformedSubSeq` with
   * specified configurations.
   * @param transform_id_sub_symbol The type of transformation to apply (e.g.,
   * equality, match, RLE).
   * @param support_values The support values for the specified transformation.
   * @param cabac_binarization The binarization configuration for CABAC coding.
   * @param subsequence_id The identifier of the subsequence.
   * @param original Flag indicating whether this is the original subsequence or
   * a derived one.
   * @param alphabet_id The identifier for the alphabet used in the
   * subsequence.
   */
  TransformedSubSeq(SupportValues::TransformIdSubsym transform_id_sub_symbol,
                    SupportValues&& support_values,
                    Binarization&& cabac_binarization,
                    core::GenSubIndex subsequence_id, bool original = true,
                    core::AlphabetId alphabet_id = core::AlphabetId::kAcgtn);

  /**
   * @brief Deserialization constructor. Initializes the subsequence from a
   * `BitReader`.
   * @param reader The `BitReader` for reading binary data.
   * @param subsequence_id The identifier of the subsequence.
   * @param alphabet_id The identifier for the alphabet used in the
   * subsequence.
   */
  TransformedSubSeq(util::BitReader& reader, core::GenSubIndex subsequence_id,
                    core::AlphabetId alphabet_id = core::AlphabetId::kAcgtn);

  /**
   * @brief Serializes the subsequence to a `BitWriter`.
   * @param writer The `BitWriter` for serializing the data.
   */
  void write(util::BitWriter& writer) const;

  /**
   * @brief Retrieves the transformation ID for the subsequence.
   * @return The transformation ID (e.g., equality, match, RLE).
   */
  [[nodiscard]] SupportValues::TransformIdSubsym GetTransformIdSubsym() const;

  /**
   * @brief Retrieves the support values for the current transformation.
   * @return The `SupportValues` object.
   */
  [[nodiscard]] const SupportValues& GetSupportValues() const;

  /**
   * @brief Retrieves the CABAC binarization configuration.
   * @return The `Binarization` object.
   */
  [[nodiscard]] const Binarization& GetBinarization() const;

  /**
   * @brief Retrieves the state variables for the current configuration.
   * @return The `StateVars` object.
   */
  [[nodiscard]] const StateVars& GetStateVars() const;

  /**
   * @brief Retrieves the modifiable state variables for the current
   * configuration.
   * @return The `StateVars` object.
   */
  StateVars& GetStateVars();

  /**
   * @brief Sets the subsequence identifier.
   * @param subsequence_id The new identifier for the subsequence.
   */
  void SetSubsequenceId(const core::GenSubIndex& subsequence_id);

  /**
   * @brief Retrieves the alphabet identifier used in the subsequence.
   * @return The `core::AlphabetID` used in the subsequence.
   */
  [[nodiscard]] core::AlphabetId GetAlphabetId() const;

  /**
   * @brief Sets the alphabet identifier for the subsequence.
   * @param alphabet_id The new alphabet identifier.
   */
  void SetAlphabetId(core::AlphabetId alphabet_id);

  /**
   * @brief Equality operator for comparing two `TransformedSubSeq` objects.
   * @param val The other `TransformedSubSeq` object to compare with.
   * @return True if both objects are equal, otherwise false.
   */
  bool operator==(const TransformedSubSeq& val) const;

  /**
   * @brief Constructs the subsequence from a JSON object.
   * @param j The JSON object containing the subsequence configuration.
   * @param last_transformed Indicates whether this is the last transformed
   * subsequence.
   */
  explicit TransformedSubSeq(nlohmann::json j, bool last_transformed);

  /**
   * @brief Converts the current subsequence to a JSON object.
   * @return A JSON representation of the subsequence.
   */
  [[nodiscard]] nlohmann::json ToJson() const;

 private:
  SupportValues::TransformIdSubsym
      transform_id_subsym_;  //!< @brief The type of transformation applied.
  SupportValues support_values_;  //!< @brief Support values associated with the
                                  //!< transformation.
  Binarization cabac_binarization_;  //!< @brief Binarization configuration for
                                     //!< CABAC coding.
  StateVars state_vars_;  //!< @brief State variables for the transformation.

  core::GenSubIndex subsequence_id_ =
      core::gen_sub::kPositionFirst;  //!< @brief Identifier for the
                                      //!< subsequence.
  core::AlphabetId alphabet_id_ =
      core::AlphabetId::kAcgtn;  //!< @brief Identifier for the alphabet used.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUB_SEQ_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
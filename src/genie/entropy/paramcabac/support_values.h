/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `SupportValues` class, which handles support values for
 * transformations in CABAC entropy coding.
 * @details This class manages configuration values used in various
 * transformations for CABAC coding. It includes parameters like symbol Size,
 * coding order, and flags for shared subsymbols. The class provides
 *          functionality to serialize, deserialize, and convert these values to
 * and from JSON.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// -----------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Stores support values for CABAC transformations.
 * @details This class is used to define parameters for transformations applied
 * to symbols in CABAC entropy coding. It includes properties for coding symbol
 * Size, coding order, and flags for sharing symbol LUTs and previous states
 * across transformations.
 */
class SupportValues final {
 public:
  /**
   * @brief Enumeration of transformation types for subsymbols.
   */
  enum class TransformIdSubsym : uint8_t {
    NO_TRANSFORM = 0,   //!< No transformation applied.
    LUT_TRANSFORM = 1,  //!< Look-Up Table transformation.
    DIFF_CODING = 2     //!< Differential coding transformation.
  };

  /**
   * @brief Default constructor. Initializes all values to their default
   * states.
   */
  SupportValues();

  /**
   * @brief Constructs `SupportValues` with specified symbol sizes, coding
   * order, and sharing flags.
   * @param output_symbol_size Size of the output symbols.
   * @param coding_subsym_size Size of the coding subsymbols.
   * @param coding_order Order in which symbols are coded.
   * @param share_subsym_lut_flag Flag indicating if subsymbols share a
   * Look-Up Table (LUT).
   * @param share_subsym_prv_flag Flag indicating if subsymbols share a
   * previous state for context.
   */
  SupportValues(uint8_t output_symbol_size, uint8_t coding_subsym_size,
                uint8_t coding_order, bool share_subsym_lut_flag = true,
                bool share_subsym_prv_flag = true);

  /**
   * @brief Deserialization constructor. Reads support values from a binary
   * reader.
   * @param transform_id_subsym The type of transformation applied to the
   * subsequence.
   * @param reader The `BitReader` used for binary deserialization.
   */
  SupportValues(TransformIdSubsym transform_id_subsym, util::BitReader& reader);

  /**
   * @brief Virtual destructor.
   */
  virtual ~SupportValues() = default;

  /**
   * @brief Serializes the support values to a binary writer.
   * @param transform_id_subsym The type of transformation to apply.
   * @param writer The `BitWriter` used for binary serialization.
   */
  void write(TransformIdSubsym transform_id_subsym,
             util::BitWriter& writer) const;

  /**
   * @brief Retrieves the output symbol Size.
   * @return The Size of output symbols in bits.
   */
  [[nodiscard]] uint8_t GetOutputSymbolSize() const;

  /**
   * @brief Retrieves the Size of coding subsymbols.
   * @return The Size of coding subsymbols in bits.
   */
  [[nodiscard]] uint8_t GetCodingSubsymSize() const;

  /**
   * @brief Retrieves the order of coding.
   * @return The order in which symbols are coded.
   */
  [[nodiscard]] uint8_t GetCodingOrder() const;

  /**
   * @brief Checks if subsymbols share a Look-Up Table (LUT).
   * @return True if subsymbols share a LUT, otherwise false.
   */
  [[nodiscard]] bool GetShareSubsymLutFlag() const;

  /**
   * @brief Checks if subsymbols share the same previous state for context.
   * @return True if subsymbols share the same previous state, otherwise
   * false.
   */
  [[nodiscard]] bool GetShareSubsymPrvFlag() const;

  /**
   * @brief Equality operator for comparing two `SupportValues` objects.
   * @param val The other `SupportValues` object to compare with.
   * @return True if both objects are equal, otherwise false.
   */
  bool operator==(const SupportValues& val) const;

  /**
   * @brief Constructs the support values from a JSON object.
   * @param j The JSON object containing the support values.
   * @param transform_id_subsym The type of transformation to apply.
   */
  explicit SupportValues(nlohmann::json j,
                         TransformIdSubsym transform_id_subsym);

  /**
   * @brief Converts the support values to a JSON object.
   * @param transform_id_subsym The type of transformation applied.
   * @return A JSON representation of the support values.
   */
  [[nodiscard]] nlohmann::json ToJson(
      TransformIdSubsym transform_id_subsym) const;

 private:
  uint8_t output_symbol_size_;  //!< @brief Size of output symbols in bits.
  uint8_t coding_subsym_size_;  //!< @brief Size of coding subsymbols in bits.
  uint8_t coding_order_;        //!< @brief Order of coding for symbols.
  bool share_subsym_lut_flag_;  //!< @brief Flag indicating if subsymbols share
                                //!< a Look-Up Table (LUT).
  bool share_subsym_prv_flag_;  //!< @brief Flag indicating if subsymbols share
                                //!< a previous state for context.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

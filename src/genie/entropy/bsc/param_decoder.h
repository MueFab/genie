/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines a BSC-specific descriptor parameter configuration for the
 * decoder.
 * @details This file contains the implementation of the `DecoderRegular` class
 * used for configuring and managing BSC-based entropy decoding parameters. The
 * `DecoderRegular` class extends the base MPEG-G descriptor-present decoder and
 * adds support for managing subsequence configurations tailored for BSC.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_PARAM_DECODER_H_
#define SRC_GENIE_ENTROPY_BSC_PARAM_DECODER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/parameter/descriptor_present/decoder_regular.h"
#include "genie/entropy/bsc/subsequence.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

/**
 * @brief Class representing a BSC-specific descriptor parameter configuration
 * for a decoder.
 * @details The `DecoderRegular` class handles the configurations specific to
 * BSC (Block Symbol Coding) for entropy decoding. It extends the functionality
 * provided by `core::parameter::desc_pres::DecoderRegular`.
 */
class DecoderRegular : public core::parameter::desc_pres::DecoderRegular {
 protected:
  std::vector<Subsequence>
      descriptor_subsequence_cfgs_;  //!< @brief List of subsequence
                                     //!< configurations.

 public:
  static const uint8_t mode_bsc_ =
      3;  //!< @brief Mode identifier for BSC decoders.

  /**
   * @brief Check for equality with another decoder.
   * @param dec Pointer to the other decoder object.
   * @return True if the decoders are equivalent.
   */
  bool Equals(const Decoder* dec) const override;

  /**
   * @brief Construct a new DecoderRegular object with default parameters.
   */
  DecoderRegular();

  /**
   * @brief Construct a new DecoderRegular object with a given descriptor.
   * @param desc Descriptor value.
   */
  explicit DecoderRegular(core::GenDesc desc);

  /**
   * @brief Construct a new DecoderRegular object from a BitReader.
   * @param desc Descriptor value.
   * @param reader BitReader to extract the parameters.
   */
  explicit DecoderRegular(core::GenDesc desc, util::BitReader& reader);

  /**
   * @brief Set the subsequence configuration at a specified index.
   * @param index Index in the configuration list.
   * @param cfg Configuration to set.
   */
  void SetSubsequenceCfg(uint8_t index, Subsequence&& cfg);

  /**
   * @brief Get the subsequence configuration at a specified index.
   * @param index Index to retrieve.
   * @return Reference to the subsequence configuration.
   */
  [[maybe_unused]] [[nodiscard]] const Subsequence& GetSubsequenceCfg(
      uint8_t index) const;

  /**
   * @brief Get the subsequence configuration at a specified index.
   * @param index Index to retrieve.
   * @return Modifiable reference to the subsequence configuration.
   */
  [[maybe_unused]] Subsequence& GetSubsequenceCfg(uint8_t index);

  /**
   * @brief Clone this decoder configuration.
   * @return Unique pointer to the new cloned object.
   */
  [[nodiscard]] std::unique_ptr<core::parameter::desc_pres::Decoder> Clone()
      const override;

  /**
   * @brief Factory method to create a BSC-specific decoder configuration.
   * @param desc Descriptor value.
   * @param reader BitReader to extract the parameters.
   * @return Unique pointer to the newly created decoder configuration.
   */
  static std::unique_ptr<core::parameter::desc_pres::DecoderRegular> Create(
      genie::core::GenDesc desc, util::BitReader& reader);

  /**
   * @brief Write the configuration parameters to a BitWriter.
   * @param writer BitWriter to write the parameters to.
   */
  void Write(util::BitWriter& writer) const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_PARAM_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

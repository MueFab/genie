/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Header file for the ZSTD entropy decoder parameter module.
 * @details This file declares the `DecoderRegular` class, which provides
 * configuration handling for the ZSTD entropy decoding process. It handles
 * subsequence parameter configurations for the ZSTD compression algorithm and
 * integrates with the GENIE parameter system.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_ZSTD_PARAM_DECODER_H_
#define SRC_GENIE_ENTROPY_ZSTD_PARAM_DECODER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/parameter/descriptor_present/decoder_regular.h"
#include "genie/entropy/zstd/subsequence.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::zstd {

constexpr uint8_t kModeZstd = 2;

/**
 * @brief Class representing a ZSTD-specific descriptor parameter configuration
 * for a decoder.
 * @details This class extends the GENIE regular decoder configuration class to
 * provide ZSTD-specific parameters for managing descriptor subsequences. It
 * stores and manages individual subsequence configurations required during the
 * decoding of entropy-coded data using the ZSTD algorithm.
 */
class DecoderRegular final : public core::parameter::desc_pres::DecoderRegular {
 protected:
  std::vector<Subsequence>
      descriptor_subsequence_configs_;  //!< @brief List of subsequence
                                        //!< configurations.

 public:
  /**
   * @brief Compare the equality of two decoder configurations.
   * @param dec Pointer to the other decoder object.
   * @return True if the decoders are equivalent.
   */
  bool Equals(const Decoder* dec) const override;

  /**
   * @brief Construct a new default DecoderRegular object.
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
  [[nodiscard]] const Subsequence& GetSubsequenceCfg(uint8_t index) const;

  /**
   * @brief Get the subsequence configuration at a specified index.
   * @param index Index to retrieve.
   * @return Modifiable reference to the subsequence configuration.
   */
  Subsequence& GetSubsequenceCfg(uint8_t index);

  /**
   * @brief Clone this decoder configuration.
   * @return Unique pointer to the new cloned object.
   */
  [[nodiscard]] std::unique_ptr<Decoder> Clone() const override;

  /**
   * @brief Factory method to create a ZSTD-specific decoder configuration.
   * @param desc Descriptor value.
   * @param reader BitReader to extract the parameters.
   * @return Unique pointer to the newly created decoder configuration.
   */
  static std::unique_ptr<core::parameter::desc_pres::DecoderRegular> create(
      core::GenDesc desc, util::BitReader& reader);

  /**
   * @brief Write the configuration parameters to a BitWriter.
   * @param writer BitWriter to write the parameters to.
   */
  void Write(util::BitWriter& writer) const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZSTD_PARAM_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

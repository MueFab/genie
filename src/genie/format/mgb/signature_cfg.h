/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_
#define SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_

// -----------------------------------------------------------------------------

#include <optional>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class SignatureCfg final {
  std::vector<uint64_t> u_cluster_signature_;        //!< @brief
  std::vector<uint8_t> u_cluster_signature_length_;  //!< @brief

  std::optional<uint8_t> u_signature_size_;  //!< @brief
  uint8_t base_bits_;

 public:
  /**
   * @brief
   * @param other
   * @return
   */
  bool operator==(const SignatureCfg& other) const;

  /**
   * @brief
   */
  explicit SignatureCfg(const uint8_t base_bit) : base_bits_(base_bit) {}

  /**
   * @brief
   * @param reader
   * @param u_signature_size
   * @param base_bits
   */
  SignatureCfg(util::BitReader& reader, uint8_t u_signature_size,
               uint8_t base_bits);


  /**
   * @brief
   * @param u_cluster_signature
   * @param length
   */
  void AddSignature(uint64_t u_cluster_signature, uint8_t length);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

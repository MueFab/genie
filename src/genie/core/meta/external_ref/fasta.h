/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_FASTA_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_FASTA_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/core/meta/external_ref.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta::external_ref {

/**
 * @brief Represents a reference in fasta format.
 */
class Fasta final : public ExternalRef {
  std::vector<std::string>
      ref_type_other_checksums_;  //!< @brief Sequence checksums

 public:
  /**
   * @brief Construct form raw values
   * @param ref_uri URI to reference
   * @param check Checksum algorithm used
   */
  Fasta(std::string ref_uri, ChecksumAlgorithm check);

  /**
   * @brief Construct from json
   * @param json Json input
   */
  explicit Fasta(const nlohmann::json& json);

  /**
   * @brief Get Checksums of sequences
   * @return Sequence checksums
   */
  [[nodiscard]] const std::vector<std::string>& GetChecksums() const;

  /**
   * @brief
   * @return
   */
  std::vector<std::string>& GetChecksums();

  /**
   * @brief Add new checksum to the end of the list
   * @param checksum New checksum
   */
  void AddChecksum(std::string checksum);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<RefBase> Clone() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::external_ref

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_FASTA_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REGION_H_
#define SRC_GENIE_CORE_META_REGION_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/core/record/class_type.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Region on the genome, spanning many loci
 */
class Region {
  /// Sequence ID name
  uint16_t seq_id_;
  /// Record classes the region applies to
  std::vector<record::ClassType> classes_;
  /// Start position of region
  uint64_t start_pos_;
  /// End position of region
  uint64_t end_pos_;

 public:
  /**
   * @brief Construct from raw values
   * @param seq_id Reference sequence ID
   * @param start Start locus
   * @param end End locus
   * @param classes List of classes the region applies to
   */
  Region(uint16_t seq_id, uint64_t start, uint64_t end,
         std::vector<record::ClassType> classes);

  /**
   * @brief Construct form json
   * @param json Json representation
   */
  explicit Region(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Get sequence ID
   * @return Sequence ID
   */
  [[nodiscard]] uint16_t GetSeqId() const;

  /**
   * @brief Get List of active record classes
   * @return List of record classes
   */
  [[nodiscard]] const std::vector<record::ClassType>& GetClasses() const;

  /**
   * @brief Return start locus
   * @return Start locus
   */
  [[nodiscard]] uint64_t GetStartPos() const;

  /**
   * @brief Return end locus
   * @return End locus
   */
  [[nodiscard]] uint64_t GetEndPos() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REGION_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
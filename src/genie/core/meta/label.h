/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_LABEL_H_
#define SRC_GENIE_CORE_META_LABEL_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/core/meta/region.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Label metadata
 */
class Label {
  /// Name of the label
  std::string label_id_;

  /// Regions in the genome the label applies to
  std::vector<Region>regions_;

 public:
  /**
   * @brief Construct from raw values
   * @param id Name of the label
   */
  explicit Label(std::string id);

  /**
   * @brief Construct from json
   * @param json Json representation
   */
  explicit Label(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Apply label to a new region of the genome
   * @param r New region
   */
  void AddRegion(Region r);

  /**
   * @brief Return the name of the label
   * @return label id
   */
  [[nodiscard]] const std::string& GetId() const;

  /**
   * @brief Return the list of active region on the genome
   * @return Active regions of this label
   */
  [[nodiscard]] const std::vector<Region>& GetRegions() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_LABEL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
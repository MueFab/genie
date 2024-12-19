/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_INTERNAL_REF_H_
#define SRC_GENIE_CORE_META_INTERNAL_REF_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>

#include "genie/core/meta/refbase.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Information baout internal references
 */
class InternalRef final : public RefBase {
  uint16_t internal_dataset_group_id_;  //!< @brief In which dataset group the
                                        //!< reference is located
  uint16_t internal_dataset_id_;  //!< @brief In which dataset the reference
                                  //!< is located

 public:
  /**
   * @brief Return dataset group id
   * @return Dataset group id
   */
  [[nodiscard]] uint16_t GetGroupId() const;

  /**
   * @brief Return dataset id
   * @return Datset id
   */
  [[nodiscard]] uint16_t GetId() const;

  /**
   * @brief Construct from json
   * @param json Json representation
   */
  explicit InternalRef(const nlohmann::json& json);

  /**
   * @brief Construct from raw values
   * @param group_id Dataset group id
   * @param id Dataset id
   */
  InternalRef(uint16_t group_id, uint16_t id);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const override;

  /**
   * @brief Returns "internal_ref"
   * @return "internal_ref"
   */
  [[nodiscard]] const std::string& GetKeyName() const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<RefBase> Clone() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_INTERNAL_REF_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

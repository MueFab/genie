/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REFERENCE_H_
#define SRC_GENIE_CORE_META_REFERENCE_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/core/meta/refbase.h"
#include "genie/core/meta/sequence.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Reference metadata
 */
class Reference {
  /// Name of the full reference
  std::string reference_name_;

  /// Major version level
  uint32_t reference_major_version_;

  /// Minor version level
  uint32_t reference_minor_version_;

  /// Patch level
  uint32_t reference_patch_version_;

  /// List of reference sequences
  std::vector<Sequence> seqs_;

  /// Reference type specific information
  std::unique_ptr<RefBase> ref_;

  /// MPEG-G part 3 meta information
  std::string reference_metadata_value_;

 public:
  /**
   * @brief
   * @param ref
   */
  Reference(const Reference& ref);

  /**
   * @brief
   * @param ref
   */
  Reference(Reference&& ref) noexcept;

  /**
   * @brief
   * @param ref
   * @return
   */
  Reference& operator=(const Reference& ref);

  /**
   * @brief
   * @param ref
   * @return
   */
  Reference& operator=(Reference&& ref) noexcept;

  /**
   * @brief Construct from raw data
   * @param name Reference name
   * @param major Major version level
   * @param minor Minor version level
   * @param patch Patch version level
   * @param base Reference type specific information
   * @param metadata MPEG-G part 3 meta information
   */
  Reference(std::string name, uint32_t major, uint32_t minor, uint32_t patch,
            std::unique_ptr<RefBase> base, std::string metadata);

  /**
   * @brief Construct from json
   * @param json Json representation
   */
  explicit Reference(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Return sequence name
   * @return Sequence name
   */
  [[nodiscard]] const std::string& GetName() const;

  /**
   * @brief
   * @return
   */
  std::string& GetName();

  /**
   * @brief Return major version level
   * @return Major version
   */
  [[nodiscard]] uint32_t GetMajorVersion() const;

  /**
   * @brief Return minor version level
   * @return Minor version level
   */
  [[nodiscard]] uint32_t GetMinorVersion() const;

  /**
   * @brief Return patch version level
   * @return Patch version level
   */
  [[nodiscard]] uint32_t GetPatchVersion() const;

  /**
   * @brief Return list of reference sequences
   * @return Information about all reference sequences
   */
  [[nodiscard]] const std::vector<Sequence>& GetSequences() const;

  /**
   * @brief
   * @return
   */
  std::vector<Sequence>& GetSequences();

  /**
   * @brief Add a new reference sequence
   * @param s Sequence to add
   */
  void AddSequence(Sequence s);

  /**
   * @brief Get reference type specific information
   * @return Reference type specific information
   */
  [[nodiscard]] const RefBase& GetBase() const;

  /**
   * @brief Get reference type specific information
   * @return Reference type specific information
   */
  std::unique_ptr<RefBase> MoveBase();

  /**
   * @brief Return MPEG-G part 3 meta information
   * @return MPEG-G part 3 meta information
   */
  [[nodiscard]] const std::string& GetInformation() const;

  /**
   * @brief
   * @return
   */
  std::string& GetInformation();
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REFERENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

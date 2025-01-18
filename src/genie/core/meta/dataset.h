/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_DATASET_H_
#define SRC_GENIE_CORE_META_DATASET_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "genie/core/meta/access_unit.h"
#include "genie/core/meta/block_header.h"
#include "genie/core/meta/dataset_group.h"
#include "genie/core/meta/descriptor_stream.h"
#include "genie/core/meta/label.h"
#include "genie/core/meta/reference.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Metadata collection for a whole dataset
 */
class Dataset {
  std::optional<DatasetGroup>
      dataset_group_;  //!< @brief Optional information about dataset group
  std::optional<Reference>
      reference_;  //!< @brief Optional information about reference
  std::vector<Label> label_list_;  //!< @brief List of labels
  uint32_t version_;               //!< @brief Current iteration of dataset
  std::unique_ptr<BlockHeader>
      header_cfg_;                   //!< @brief Configuration of block headers
  std::string dt_metadata_value_;    //!< @brief MPEG-G part 3 metadata
  std::string dt_protection_value_;  //!< @brief MPEG-G part 3 protection data
  std::vector<AccessUnit>
      access_units_;  //!< @brief List of AU meta information
  std::vector<DescriptorStream>
      descriptor_streams_;  //!< @brief List DS meta information

 public:
  /**
   * @brief
   * @param meta
   */
  void SetMetadata(std::string meta);

  /**
   * @brief
   * @param protection
   */
  void SetProtection(std::string protection);

  /**
   * @brief Construct from raw information
   * @param version Current iteration
   * @param header_cfg Block header mode
   * @param meta MPEG-G part 3 metadata
   * @param protection MPEG-G part 3 protection data
   */
  Dataset(uint32_t version, std::unique_ptr<BlockHeader> header_cfg,
          std::string meta, std::string protection);

  /**
   * @brief
   */
  Dataset();

  /**
   * @brief
   * @param hdr
   */
  void SetHeader(std::unique_ptr<BlockHeader> hdr);

  /**
   * @brief Construct from json
   * @param json Json representation
   */
  explicit Dataset(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Return dataset group information, if any
   * @return DatasetGroup information
   */
  [[nodiscard]] const std::optional<DatasetGroup>& GetDataGroup() const;

  /**
   * @brief
   * @return
   */
  std::optional<DatasetGroup>& GetDataGroup();

  /**
   * @brief Return reference information, if any
   * @return Reference information
   */
  [[nodiscard]] const std::optional<Reference>& GetReference() const;

  /**
   * @brief
   * @return
   */
  std::optional<Reference>& GetReference();

  /**
   * @brief Return list of labels
   * @return List of labels
   */
  [[nodiscard]] const std::vector<Label>& GetLabels() const;

  /**
   * @brief
   * @return
   */
  std::vector<Label>& GetLabels();

  /**
   * @brief Return current version iteration
   * @return Version iteration
   */
  [[nodiscard]] uint32_t GetVersion() const;

  /**
   * @brief Return block header config
   * @return Block header config
   */
  [[nodiscard]] const BlockHeader& GetHeader() const;

  /**
   * @brief Return MPEG-G Part 3 meta information
   * @return MPEG-G Part 3 meta information
   */
  [[nodiscard]] const std::string& GetInformation() const;

  /**
   * @brief
   * @return
   */
  std::string& GetInformation();

  /**
   * @brief Return MPEG-G Part 3 protection information
   * @return MPEG-G Part 3 protection information
   */
  [[nodiscard]] const std::string& GetProtection() const;

  /**
   * @brief
   * @return
   */
  std::string& GetProtection();

  /**
   * @brief Return list of access unit meta information blocks
   * @return AU meta information blocks
   */
  [[nodiscard]] const std::vector<AccessUnit>& GetAUs() const;

  /**
   * @brief
   * @return
   */
  std::vector<AccessUnit>& GetAUs();

  /**
   * @brief Return list of descriptor stream meta information blocks
   * @return Descriptor stream blocks
   */
  [[nodiscard]] const std::vector<DescriptorStream>& GetDSs() const;

  /**
   * @brief
   * @return
   */
  std::vector<DescriptorStream>& GetDSs();

  /**
   * @brief Update dataset group information
   * @param dg New dataset group information
   */
  void SetDataGroup(DatasetGroup dg);

  /**
   * @brief Update reference meta information
   * @param ref New reference meta information
   */
  void SetReference(Reference ref);

  /**
   * @brief Add a new label to the dataset
   * @param l New label
   */
  void AddLabel(Label l);

  /**
   * @brief Add access unit metadata block
   * @param au Access unit metadata block
   */
  void AddAccessUnit(AccessUnit au);

  /**
   * @brief Add descriptor stream meta data block
   * @param ds New data set meta data block
   */
  void AddDescriptorStream(DescriptorStream ds);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DATASET_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 * TODO: DOCSTRING
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/meta/block_header.h"
#include "genie/core/meta/block_header/disabled.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgg/dataset_header/block_header_off_options.h"
#include "genie/format/mgg/dataset_header/block_header_on_options.h"
#include "genie/format/mgg/dataset_header/mit_class_config.h"
#include "genie/format/mgg/dataset_header/reference_options.h"
#include "genie/format/mgg/dataset_header/u_options.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class DatasetHeader final : public GenInfo {
  uint8_t group_id_;                    //!< @brief
  uint16_t id_;                         //!< @brief
  core::MpegMinorVersion version_;      //!< @brief
  bool multiple_alignment_flag_;        //!< @brief
  bool byte_offset_size_flag_;          //!< @brief
  bool non_overlapping_au_range_flag_;  //!< @brief
  bool pos_40_bits_flag_;               //!< @brief

  std::optional<dataset_header::BlockHeaderOnOptions>
      block_header_on_;  //!< @brief
  std::optional<dataset_header::BlockHeaderOffOptions>
      block_header_off_;  //!< @brief

  dataset_header::ReferenceOptions reference_options_;  //!< @brief

  core::parameter::DataUnit::DatasetType dataset_type_;  //!< @brief

  std::vector<dataset_header::MitClassConfig> mit_configs_;  //!< @brief
  bool parameters_update_flag_;                              //!< @brief
  core::AlphabetId alphabet_id_;                             //!< @brief

  uint32_t num_u_access_units_;                        //!< @brief
  std::optional<dataset_header::UOptions> u_options_;  //!< @brief

  std::vector<std::optional<uint32_t>> thresholds_;  //!< @brief

 public:
  /**
   * @brief
   * @param old
   * @param _new
   */
  void PatchRefId(uint8_t old, uint8_t _new);

  /**
   * @brief
   * @param group_id
   * @param set_id
   */
  void PatchId(uint8_t group_id, uint16_t set_id);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<core::meta::BlockHeader> decapsulate() const;

  /**
   * @brief
   * @param output
   * @param depth
   * @param max_depth
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;

  /**
   * @brief
   * @param info
   * @return
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetDatasetGroupId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetDatasetId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] core::MpegMinorVersion GetVersion() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool GetMultipleAlignmentFlag() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetByteOffsetSize() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool GetNonOverlappingAuRangeFlag() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetPosBits() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsBlockHeaderEnabled() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsMitEnabled() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsCcModeEnabled() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsOrderedBlockMode() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const dataset_header::ReferenceOptions& GetReferenceOptions()
      const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] core::parameter::DataUnit::DatasetType GetDatasetType() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<dataset_header::MitClassConfig>&
  GetMitConfigs() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool GetParameterUpdateFlag() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] core::AlphabetId GetAlphabetId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetNumUAccessUnits() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const dataset_header::UOptions& GetUOptions() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<std::optional<uint32_t>>&
  GetRefSeqThresholds() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief
   */
  DatasetHeader();

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param version
   * @param multiple_alignments_flags
   * @param byte_offset_size_flags
   * @param non_overlapping_au_range_flag
   * @param pos_40_bits_flag
   * @param dataset_type
   * @param parameters_update_flag
   * @param alphabet_id
   */
  DatasetHeader(uint8_t dataset_group_id, uint16_t dataset_id,
                core::MpegMinorVersion version, bool multiple_alignments_flags,
                bool byte_offset_size_flags, bool non_overlapping_au_range_flag,
                bool pos_40_bits_flag,
                core::parameter::DataUnit::DatasetType dataset_type,
                bool parameters_update_flag, core::AlphabetId alphabet_id);

  /**
   * @brief
   * @param reader
   */
  explicit DatasetHeader(util::BitReader& reader);

  /**
   * @brief
   * @param writer
   */
  void BoxWrite(util::BitWriter& writer) const override;

  /**
   * @brief
   * @param reference_id
   * @param seq_id
   * @param blocks_num
   * @param threshold
   */
  void AddRefSequence(uint8_t reference_id, uint16_t seq_id,
                      uint32_t blocks_num, std::optional<uint32_t> threshold);

  /**
   * @brief
   * @param num_u_access_units
   * @param u_opts
   */
  void SetUaUs(uint32_t num_u_access_units, dataset_header::UOptions u_opts);

  /**
   * @brief
   * @param config
   */
  void AddClassConfig(dataset_header::MitClassConfig config);

  /**
   * @brief
   * @param opts
   */
  void DisableBlockHeader(dataset_header::BlockHeaderOffOptions opts);

  /**
   * @brief
   */
  void DisableMit();
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

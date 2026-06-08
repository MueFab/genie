/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_HEADER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <memory>

#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/record.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/block.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class AuHeader {
  uint32_t access_unit_id_;                       //!< @brief
  uint8_t num_blocks_;                            //!< @brief
  uint8_t parameter_set_id_;                      //!< @brief
  core::record::ClassType au_type_;               //!< @brief
  uint32_t reads_count_;                          //!< @brief
  std::optional<MmCfg> mm_cfg_;                   //!< @brief
  std::optional<RefCfg> ref_cfg_;                 //!< @brief
  std::optional<AuTypeCfg> au_type_u_cfg_;        //!< @brief
  std::optional<SignatureCfg> signature_config_;  //!< @brief

 public:
  /**
   * @brief
   * @param other
   * @return
   */
  bool operator==(const AuHeader& other) const;

  /**
   * @brief
   * @param writer
   * @param write_signatures
   */
  void Write(util::BitWriter& writer, bool write_signatures) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetNumBlocks() const;

  /**
   * @brief
   */
  void BlockAdded();

  /**
   * @brief
   * @param bit_reader
   * @param parameter_sets
   * @param read_signatures
   */
  AuHeader(util::BitReader& bit_reader,
           const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
           bool read_signatures = true);

  /**
   * @brief
   */
  AuHeader();

  /**
   * @brief
   * @param access_unit_id
   * @param parameter_set_id
   * @param au_type
   * @param reads_count
   * @param dataset_type
   * @param pos_size
   * @param signature_flag
   * @param alphabet
   */
  AuHeader(uint32_t access_unit_id, uint8_t parameter_set_id,
           core::record::ClassType au_type, uint32_t reads_count,
           core::parameter::ParameterSet::DatasetType dataset_type,
           uint8_t pos_size, bool signature_flag, core::AlphabetId alphabet);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetParameterId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const AuTypeCfg& GetAlignmentInfo() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetReadCount() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] core::record::ClassType GetClass() const;

  /**
   * @brief
   * @param cfg
   */
  void SetMmCfg(const MmCfg& cfg);

  /**
   * @brief
   * @param cfg
   */
  void SetRefCfg(const RefCfg& cfg);

  /**
   * @brief
   * @return
   */
  const RefCfg& GetRefCfg() const;

  /**
   * @brief
   * @param cfg
   */
  void SetAuTypeCfg(AuTypeCfg&& cfg);

  /**
   * @brief
   * @param cfg
   */
  void SetSignatureCfg(SignatureCfg&& cfg);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_HEADER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

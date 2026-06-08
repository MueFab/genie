/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------

#include <map>
#include <vector>
#include <string>
#include "genie/format/mgb/access_unit_header.h"
#include "genie/format/mgb/block.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class AccessUnit final : public core::parameter::DataUnit {
 public:
  /**
   * @brief
   */
  [[nodiscard]] std::string DebugPrint(
      const core::parameter::EncodingSet& ps) const;

  /**
   * @brief
   * @param parameter_sets
   * @param bit_reader
   * @param lazy_payload
   */
  explicit AccessUnit(
      const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
      util::BitReader& bit_reader, bool lazy_payload = false);

  /**
   * @brief
   * @param bit_reader
   */
  void LoadPayload(util::BitReader& bit_reader);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t GetPayloadSize() const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const override;

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
  AccessUnit(uint32_t access_unit_id, uint8_t parameter_set_id,
             core::record::ClassType au_type, uint32_t reads_count,
             DatasetType dataset_type, uint8_t pos_size, bool signature_flag,
             core::AlphabetId alphabet);

  /**
   * @brief
   * @param block
   */
  void AddBlock(Block block);

  /**
   * @brief
   * @return
   */
  std::vector<Block>& GetBlocks();

  /**
   * @brief
   * @return
   */
  AuHeader& GetHeader();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const AuHeader& GetHeader() const;

  /**
   * @brief
   * @param output
   */
  void PrintDebug(std::ostream& output, uint8_t, uint8_t) const override;

  /**
   * @brief
   * @param h
   * @param b
   */
  AccessUnit(AuHeader h, std::vector<Block> b);

 private:
  AuHeader header;  //!< @brief

  std::vector<Block> blocks;  //!< @brief

  size_t payloadbytes;  //!< @brief
  size_t qv_payloads;   //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

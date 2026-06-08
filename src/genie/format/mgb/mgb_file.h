/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_MGB_FILE_H_
#define SRC_GENIE_FORMAT_MGB_MGB_FILE_H_

// -----------------------------------------------------------------------------

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/data_unit_factory.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class MgbFile {
  ///
  std::vector<std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>>
      units_;

  ///
  std::istream* file_;

  ///
  std::unique_ptr<util::BitReader> reader_;

  ///
  std::map<size_t, core::parameter::EncodingSet> parameter_sets_;

  /**
   * @brief
   * @param u
   * @return
   */
  static uint8_t data_unit_order(const core::parameter::DataUnit& u);

  /**
   * @brief
   * @tparam Lambda
   * @param u1
   * @param u2
   * @param lambda
   * @return
   */
  template <typename Lambda>
  static bool base_sorter(
      const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>& u1,
      const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>& u2,
      Lambda lambda);

 public:
  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param unit
   */
  void AddUnit(std::unique_ptr<core::parameter::DataUnit> unit);

  /**
   * @brief
   */
  MgbFile();

  /**
   * @brief
   * @param file
   */
  explicit MgbFile(std::istream* file);

  /**
   * @brief
   * @param output
   * @param max_depth
   */
  void print_debug(std::ostream& output, uint8_t max_depth = 100) const;

  /**
   * @brief
   */
  void sort_by_class();

  /**
   * @brief
   */
  void sort_by_position();

  /**
   * @brief
   * @param type
   */
  void remove_class(core::record::ClassType type);

  /**
   * @brief
   * @param ref_id
   * @param start_pos
   * @param end_pos
   */
  void select_mapping_range(uint16_t ref_id, uint64_t start_pos,
                            uint64_t end_pos);

  /**
   * @brief
   */
  void remove_unused_parametersets();

  /**
   * @brief
   * @param type
   * @param descriptor
   * @param param_sets
   * @return
   */
  std::vector<Block> ExtractDescriptor(core::record::ClassType type,
                                       core::GenDesc descriptor,
                                       const std::vector<uint8_t>& param_sets);

  /**
   * @brief
   * @param param_sets
   */
  void ClearAuBlocks(const std::vector<uint8_t>& param_sets);

  /**
   * @brief
   * @param param_sets
   * @return
   */
  std::vector<std::unique_ptr<core::parameter::ParameterSet>> ExtractParameters(
      const std::vector<uint8_t>& param_sets);

  /**
   * @brief
   * @param param_sets
   * @return
   */
  std::vector<std::unique_ptr<AccessUnit>> ExtractAUs(
      const std::vector<uint8_t>& param_sets);

  /**
   * @brief
   * @param multiple_alignments
   * @param pos40
   * @param dataset_type
   * @param alphabet
   * @return
   */
  [[nodiscard]] std::vector<uint8_t> collect_param_ids(
      bool multiple_alignments, bool pos40,
      core::parameter::DataUnit::DatasetType dataset_type,
      core::AlphabetId alphabet) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#include "genie/format/mgb/mgb_file.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_MGB_FILE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

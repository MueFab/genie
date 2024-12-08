/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_GROUP_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_GROUP_H_

// -----------------------------------------------------------------------------

#include <list>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/core/record/record.h"
#include "genie/format/sam/sam_to_mgrec/sam_record.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

/**
 * @brief Helping structure to sort the records
 */
class SamRecordGroup {
 public:
  /**
   * @brief
   */
  enum class TemplateType : uint8_t {
    SINGLE = 0,
    PAIRED_1 = 1,
    PAIRED_2 = 2,
    PAIRED_UNKNOWN = 3,
    TOTAL_INDICES = 4,  // Not used
  };

  /**
   * @brief
   */
  enum class MappingType : uint8_t {
    UNMAPPED = 0,
    PRIMARY = 1,
    NONPRIMARY = 2,
    TOTAL_INDICES = 3,  // Not used
  };

 private:
  std::vector<std::vector<std::list<SamRecord>>> data_;  //!< @brief

  /**
   * @brief
   * @param rec
   * @param r1
   * @param r2
   * @param paired_end
   * @param force_split
   */
  static void AddAlignment(core::record::Record& rec, SamRecord* r1,
                           SamRecord* r2, bool paired_end,
                           bool force_split = false);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool CheckIfPaired() const;

  /**
   * @brief
   * @return
   */
  genie::core::record::ClassType CheckClassTypeSingle();

  /**
   * @brief
   * @param temp_type
   * @param map_type
   */
  void RemoveDuplicatesPaired(TemplateType temp_type, MappingType map_type);

  /**
   * @brief
   * @param temp_type
   * @return
   */
  [[nodiscard]] size_t PrimaryTemplateCount(TemplateType temp_type) const;

  /**
   * @brief
   * @param map_type
   * @return
   */
  [[nodiscard]] size_t MappingCount(MappingType map_type) const;

  /**
   * @brief
   */
  void RemoveAmbiguousSecondaryAlignments();

  /**
   * @brief
   */
  void MoveSecondaryAlignments();

  /**
   * @brief
   * @param unknown_count
   * @param read_1_count
   * @param read_2_count
   */
  void GuessUnknownReadOrder(size_t& unknown_count, size_t& read_1_count,
                             size_t& read_2_count);

  /**
   * @brief
   * @return
   */
  core::record::ClassType CheckClassTypePaired();

  /**
   * @brief
   * @return
   */
  std::pair<SamRecord*, SamRecord*> GetReadTuple();

  /**
   * @brief
   * @param cls
   * @return
   */
  [[nodiscard]] bool IsR1First(
      const std::pair<bool, core::record::ClassType>& cls) const;

  /**
   * @brief
   * @param rec
   * @param r1
   */
  static void AddSegment(core::record::Record& rec, SamRecord* r1);

 public:
  /**
   * @brief
   */
  SamRecordGroup();

  /**
   * @brief
   * @param flags
   * @return
   */
  static std::tuple<bool, uint8_t> ConvertFlags2Mpeg(uint16_t flags);

  /**
   * @brief
   * @return
   */
  std::pair<bool, core::record::ClassType> validate();

  /**
   * @brief
   * @param rec
   */
  void AddRecord(SamRecord&& rec);

  /**
   * @brief
   * @param records
   * @param create_same_rec
   */
  void convert(std::list<core::record::Record>& records,
               bool create_same_rec = false);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_GROUP_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

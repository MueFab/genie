/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_RECORD_H_
#define SRC_GENIE_CORE_RECORD_RECORD_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/record/alignment_box.h"
#include "genie/core/record/alignment_external.h"
#include "genie/core/record/alignment_shared_data.h"
#include "genie/core/record/class_type.h"
#include "genie/core/record/segment.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 *  @brief
 */
class Record {
  uint8_t number_of_template_segments_{};      //!< @brief
  std::vector<Segment> reads_;                 //!< @brief
  std::vector<AlignmentBox> alignment_info_;   //!< @brief
  ClassType class_id_{ClassType::kNone};       //!< @brief
  std::string read_group_;                     //!< @brief
  bool read_1_first_{};                        //!< @brief
  AlignmentSharedData shared_alignment_info_;  //!< @brief
  uint8_t qv_depth_{};                         //!< @brief
  std::string read_name_;                      //!< @brief
  uint8_t flags_{};                            //!< @brief

  std::unique_ptr<AlignmentExternal> more_alignment_info_;  //!< @brief

 public:
  /**
   * @brief
   */
  void PatchRefId(size_t ref_id);

  /**
   * @brief
   */
  Record();

  /**
   * @brief
   * @param number_of_template_segments
   * @param au_type_cfg
   * @param read_name
   * @param read_group
   * @param flags
   * @param is_read_1_first
   */
  Record(uint8_t number_of_template_segments, ClassType au_type_cfg,
         std::string&& read_name, std::string&& read_group, uint8_t flags,
         bool is_read_1_first = true);

  /**
   * @brief
   * @param rec
   */
  Record(const Record& rec);

  /**
   * @brief
   * @param rec
   */
  Record(Record&& rec) noexcept;

  /**
   * @brief
   */
  ~Record() = default;

  /**
   * @brief
   * @param rec
   * @return
   */
  Record& operator=(const Record& rec);

  /**
   * @brief
   * @param rec
   * @return
   */
  Record& operator=(Record&& rec) noexcept;

  /**
   * @brief
   * @param reader
   */
  explicit Record(util::BitReader& reader);

  /**
   * @brief
   * @param rec
   */
  void AddSegment(Segment&& rec);

  /**
   * @brief
   * @param seq_id
   * @param rec
   */
  void AddAlignment(uint16_t seq_id, AlignmentBox&& rec);

  /**
   * @brief
   * @return
   */
  std::vector<Segment>& GetSegments();

  /**
   * @brief
   */
  void SwapSegmentOrder();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<Segment>& GetSegments() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t GetNumberOfTemplateSegments() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<AlignmentBox>& GetAlignments() const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetFlags() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] ClassType GetClassId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetName() const;

  /**
   * @brief
   * @param name
   */
  void SetName(const std::string& name);

  /**
   * @brief
   * @param depth
   */
  void SetQvDepth(uint8_t depth);

  /**
   * @brief
   * @param val
   */
  void SetRead1First(bool val);

  /**
   * @brief
   * @param type
   */
  void SetClassType(ClassType type);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetGroup() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const AlignmentSharedData& GetAlignmentSharedData() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const AlignmentExternal& GetAlignmentExternal() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsRead1First() const;

  /**
   * @brief
   * @param cigar
   * @return
   */
  static uint64_t GetLengthOfCigar(const std::string& cigar);

  /**
   * @brief
   * @param alignment
   * @param split
   * @return
   */
  [[nodiscard]] size_t GetMappedLength(size_t alignment, size_t split) const;

  /**
   * @brief
   * @param id
   * @param b
   */
  void SetAlignment(size_t id, AlignmentBox&& b);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::pair<size_t, size_t> GetTemplatePosition() const;

  /**
   * @brief
   * @param alignment
   * @param split
   * @return
   */
  [[nodiscard]] size_t GetPosition(size_t alignment, size_t split) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool GetRead1First() const;

  /**
   * @brief
   */
  void SetMoreAlignmentInfo(
      std::unique_ptr<AlignmentExternal> more_alignment_info);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_RECORD_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

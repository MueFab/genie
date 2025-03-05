/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_RECORD_H_
#define SRC_GENIE_CORE_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "alignment_box.h"
#include "alignment_external.h"
#include "alignment_shared_data.h"
#include "class_type.h"
#include "genie/core/constants.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "segment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief Class representing a genomic record
 *
 * Contains segments, alignments, and metadata for one or more template segments.
 */
class Record {
 private:
  uint8_t number_of_template_segments_{};    //!< @brief
  std::vector<Segment> reads_;               //!< @brief
  std::vector<AlignmentBox> alignment_info_;  //!< @brief
  ClassType class_id_{ClassType::kNone};     //!< @brief
  std::string read_group_;                   //!< @brief
  bool read_1_first_{};                      //!< @brief
  AlignmentSharedData shared_alignment_info_;  //!< @brief
  uint8_t qv_depth_{};                       //!< @brief
  std::string read_name_;                    //!< @brief
  uint8_t flags_{};                          //!< @brief

  std::unique_ptr<AlignmentExternal> more_alignment_info_;  //!< @brief

 public:
  /**
   * @brief
   */
  void PatchRefID(size_t refID);

  /**
   * @brief
   */
  Record();

  /**
   * @brief
   * @param _number_of_template_segments
   * @param _auTypeCfg
   * @param _read_name
   * @param _read_group
   * @param _flags
   * @param _is_read_1_first
   */
  Record(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::string&& _read_name,
         std::string&& _read_group, uint8_t _flags, bool _is_read_1_first = true);

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
   * @param _seq_id
   * @param rec
   */
  void AddAlignment(uint16_t _seq_id, AlignmentBox&& rec);

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
  [[nodiscard]] ClassType GetClassID() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetName() const;

  /**
   * @brief
   * @param _name
   */
  void SetName(const std::string& _name);

  /**
   * @brief
   * @param depth
   */
  void SetQVDepth(uint8_t depth);

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
   * @return
   */
  void SetMoreAlignmentInfo(std::unique_ptr<AlignmentExternal> _more_alignment_info);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

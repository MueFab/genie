/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_
#define SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/parameter/computed_ref.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/parameter/descriptor_sub_sequence_cfg.h"
#include "genie/core/parameter/quality_values.h"
#include "genie/core/record/class_type.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

namespace stats {
class PerfStats;
}

// -----------------------------------------------------------------------------

namespace parameter {

class EncodingSet {
 public:
  /**
   * @brief
   */
  struct SignatureCfg {
    std::optional<uint8_t> signature_length;  //!< @brief

    /**
     * @brief
     * @param cfg
     * @return
     */
    bool operator==(const SignatureCfg& cfg) const;
  };

 private:
  DataUnit::DatasetType dataset_type_;                             //!< @brief
  AlphabetId alphabet_id_;                                         //!< @brief
  uint32_t read_length_;                                           //!< @brief
  uint8_t number_of_template_segments_minus1_;                     //!< @brief
  uint32_t max_au_data_unit_size_;                                 //!< @brief
  bool pos_40_bits_flag_;                                          //!< @brief
  uint8_t qv_depth_;                                               //!< @brief
  uint8_t as_depth_;                                               //!< @brief
  std::vector<record::ClassType> class_i_ds_;                      //!< @brief
  std::vector<DescriptorSubSequenceCfg> descriptors_;              //!< @brief
  std::vector<std::string> read_group_index_ds_;                   //!< @brief
  bool multiple_alignments_flag_;                                  //!< @brief
  bool spliced_reads_flag_;                                        //!< @brief
  uint32_t reserved_{};                                            //!< @brief
  std::optional<SignatureCfg> signature_cfg_;                      //!< @brief
  std::vector<std::unique_ptr<QualityValues>> qv_coding_configs_;  //!< @brief
  std::optional<ComputedRef> computed_reference_;                  //!< @brief

  /**
   * @brief
   * @param ps
   * @return
   */
  [[nodiscard]] bool QualityValueCmp(const EncodingSet& ps) const;

 public:
  [[nodiscard]] AlphabetId GetAlphabetId() const { return alphabet_id_; }

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const ComputedRef& GetComputedRef() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsComputedReference() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t GetNumberTemplateSegments() const;

  /**
   * @brief
   * @param computed_reference
   */
  void SetComputedRef(const ComputedRef& computed_reference);

  /**
   * @brief
   * @param class_id
   * @param conf
   */
  void AddClass(record::ClassType class_id,
                std::unique_ptr<QualityValues> conf);

  /**
   * @brief
   * @param index
   * @param descriptor
   */
  void SetDescriptor(GenDesc index, DescriptorSubSequenceCfg&& descriptor);

  /**
   * @brief
   * @param index
   * @return
   */
  [[nodiscard]] const DescriptorSubSequenceCfg& GetDescriptor(
      GenDesc index) const;

  /**
   * @brief
   * @param read_group_id
   */
  void AddGroup(std::string&& read_group_id);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] DataUnit::DatasetType GetDatasetType() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetPosSize() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool HasMultipleAlignments() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetReadLength() const;

  /**
   * @brief
   * @param other
   * @return
   */
  EncodingSet& operator=(const EncodingSet& other);

  /**
   * @brief
   * @param other
   * @return
   */
  EncodingSet& operator=(EncodingSet&& other) noexcept;

  /**
   * @brief
   * @param other
   */
  EncodingSet(const EncodingSet& other);

  /**
   * @brief
   * @param other
   */
  EncodingSet(EncodingSet&& other) noexcept;

  /**
   * @brief
   * @param type
   * @return
   */
  [[nodiscard]] const QualityValues& GetQvConfig(record::ClassType type) const;

  /**
   * @brief
   * @param qv
   */
  void SetQvDepth(uint8_t qv);

  /**
   * @brief
   * @param ps
   * @return
   */
  bool operator==(const EncodingSet& ps) const;

  /**
   * @brief
   */
  void ActivateSignature();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsSignatureActivated() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsSignatureConstLength() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetSignatureConstLength() const;

  /**
   * @brief
   * @param length
   */
  void SetSignatureLength(uint8_t length);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param dataset_type
   * @param alphabet_id
   * @param read_length
   * @param paired_end
   * @param pos_40_bits_flag
   * @param qv_depth
   * @param as_depth
   * @param multiple_alignments_flag
   * @param spliced_reads_flag
   */
  EncodingSet(DataUnit::DatasetType dataset_type, AlphabetId alphabet_id,
              uint32_t read_length, bool paired_end, bool pos_40_bits_flag,
              uint8_t qv_depth, uint8_t as_depth, bool multiple_alignments_flag,
              bool spliced_reads_flag);

  /**
   * @brief
   */
  EncodingSet();

  /**
   * @brief
   * @param bit_reader
   */
  explicit EncodingSet(util::BitReader& bit_reader);
};

/**
 * @brief MPEG-G Part 2 parameter set
 */
class ParameterSet final : public DataUnit {
 public:
  /**
   * @brief Read parameter set from bitstream
   * @param bit_reader Stream to read from
   */
  explicit ParameterSet(util::BitReader& bit_reader);

  /**
   * @brief Construct from raw values
   * @param parameter_set_id ID for this parameter set
   * @param parent_parameter_set_id ID of parent parameter set (set equal to
   * _parameter_set_ID for no parent)
   * @param dataset_type
   * @param alphabet_id
   * @param read_length
   * @param paired_end
   * @param pos_40_bits_flag
   * @param qv_depth
   * @param as_depth
   * @param multiple_alignments_flag
   * @param spliced_reads_flag
   */
  ParameterSet(uint8_t parameter_set_id, uint8_t parent_parameter_set_id,
               DatasetType dataset_type, AlphabetId alphabet_id,
               uint32_t read_length, bool paired_end, bool pos_40_bits_flag,
               uint8_t qv_depth, uint8_t as_depth,
               bool multiple_alignments_flag, bool spliced_reads_flag);

  /**
   * @brief
   */
  ParameterSet();

  /**
   * @brief
   * @param parameter_set_id
   * @param parent_parameter_set_id
   * @param set
   */
  ParameterSet(uint8_t parameter_set_id, uint8_t parent_parameter_set_id,
               EncodingSet set);

  /**
   * @brief
   * @param parameter_set
   * @return
   */
  bool operator==(const ParameterSet& parameter_set) const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief
   * @param id
   */
  void SetId(uint8_t id);

  /**
   * @brief
   * @param id
   */
  void SetParentId(uint8_t id);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetParentId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetId() const;

  /**
   * @brief
   * @return
   */
  EncodingSet& GetEncodingSet() { return set_; }

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const EncodingSet& GetEncodingSet() const { return set_; }

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetLength() const;

  /**
   * @brief
   * @param output
   */
  void PrintDebug(std::ostream& output, uint8_t, uint8_t) const override;

 private:
  uint8_t parameter_set_id_;         //!< @brief
  uint8_t parent_parameter_set_id_;  //!< @brief

  EncodingSet set_;  //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace parameter
}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

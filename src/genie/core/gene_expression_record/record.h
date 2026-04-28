/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#ifndef SRC_GENIE_CORE_GENE_EXPRESSION_RECORD_RECORD_H_
#define SRC_GENIE_CORE_GENE_EXPRESSION_RECORD_RECORD_H_
// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_writer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::gene_expression {

class ExpressionAttribute {
 private:
  std::string attr_name_;  //!< @brief Attribute name
  DataType attr_type_;     //!< @brief Attribute data type
  uint8_t attr_array_len_;      //!< @brief Attribute array length
  std::vector<std::vector<std::vector<uint8_t>>> attr_values_;  //!< @brief Attribute values

 public:
  ExpressionAttribute() = default;
  ExpressionAttribute(std::string& attr_name, DataType attr_type, uint8_t attr_array_len)
      : attr_name_(attr_name),
        attr_type_(attr_type),
        attr_array_len_(attr_array_len)
      {}

    // Accessors
  [[nodiscard]] const std::string& GetAttrName() const {
    return attr_name_;
  }
  [[nodiscard]] DataType GetAttrType() const {
    return attr_type_;
  }
  [[nodiscard]] uint8_t GetAttrArrayLen() const {
    return attr_array_len_;
  }
  [[nodiscard]] const std::vector<std::vector<std::vector<uint8_t>>>& GetAttrValues() const {
    return attr_values_;
  }

  void SetAttrValues(const std::vector<std::vector<std::vector<uint8_t>>>& values) {
    attr_values_ = values;
  }
};

/**
 * @brief Class representing a gene expression record
 */
class Record {
 private:
  uint64_t feature_index;   //!< @brief Feature index
  uint8_t feature_name_len;  //!< @brief Length of the feature name
  std::string feature_name;  //!< @brief Feature name
  uint32_t sample_index_from;  //!< @brief Sample index from
  uint32_t sample_count;       //!< @brief Sample count
  uint8_t expr_attr_count;     //!< @brief Expression attribute count
  std::vector<ExpressionAttribute> expression_attributes;  //!< @brief Expression attributes
  uint8_t linked_record;  //!< @brief Flag indicating if record is linked
  uint8_t link_name_len;  //!< @brief Length of the link name
  std::string link_name;  //!< @brief Link name
  uint8_t reference_box_ID;  //!< @brief Reference box identifier

 public:
  /**
   * @brief Default constructor
   */
  Record() = default;

  explicit Record(genie::util::BitReader& reader) {
    Read(reader);
  }

  /**
   * @brief Reads the gene expression record from a bit reader
   * @param reader The bit reader to read from
   * @return True if read was successful, false otherwise
   */
  bool Read(genie::util::BitReader& reader);

  /**
   * @brief Writes the gene expression record to a bit writer
   * @param writer The bit writer to write to
   */
  void Write(genie::core::Writer& writer);

  /**
   * @brief Gets the feature index
   * @return The feature index
   */
  [[nodiscard]] uint64_t GetFeatureIndex() const {
    return feature_index;
  }

  /**
   * @brief Gets the feature name length
   * @return The length of the feature name
   */
  [[nodiscard]] uint8_t GetFeatureNameLen() const {
    return feature_name_len;
  }

  /**
   * @brief Gets the feature name
   * @return The feature name
   */
  [[nodiscard]] const std::string& GetFeatureName() const {
    return feature_name;
  }

  /**
   * @brief Gets the sample index from
   * @return The sample index from
   */
  [[nodiscard]] uint32_t GetSampleIndexFrom() const {
    return sample_index_from;
  }

  /**
   * @brief Gets the sample count
   * @return The sample count
   */
  [[nodiscard]] uint32_t GetSampleCount() const {
    return sample_count;
  }

  /**
   * @brief Gets the expression attribute count
   * @return The expression attribute count
   */
  [[nodiscard]] uint8_t GetExprAttrCount() const {
    return expr_attr_count;
  }

  /**
   * @brief Gets the expression attributes
   * @return Reference to the vector of expression attributes
   */
  [[nodiscard]] std::vector<ExpressionAttribute>& GetExpressionAttributes() {
    return expression_attributes;
  }

  /**
   * @brief Gets the expression attributes (const)
   * @return Const reference to the vector of expression attributes
   */
  [[nodiscard]] const std::vector<ExpressionAttribute>& GetExpressionAttributes() const {
    return expression_attributes;
  }

  /**
   * @brief Gets the linked record flag
   * @return The linked record flag
   */
  [[nodiscard]] uint8_t GetLinkedRecord() const {
    return linked_record;
  }

  /**
   * @brief Gets the link name length
   * @return The length of the link name
   */
  [[nodiscard]] uint8_t GetLinkNameLen() const {
    return link_name_len;
  }

  /**
   * @brief Gets the link name
   * @return The link name
   */
  [[nodiscard]] const std::string& GetLinkName() const {
    return link_name;
  }

  /**
   * @brief Gets the reference box ID
   * @return The reference box ID
   */
  [[nodiscard]] uint8_t GetReferenceBoxID() const {
    return reference_box_ID;
  }
};

}  // namespace genie::core::record::gene_expression

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_GENE_EXPRESSION_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

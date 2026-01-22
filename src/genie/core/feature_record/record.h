/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_FEATURE_RECORD_H_
#define SRC_GENIE_CORE_RECORD_FEATURE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_writer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::feature {

/**
 * @brief Class for handling information fields in feature records
 */
class FeatureFields {
 public:
  /**
   * @brief Custom type definition for field values
   */
  using CustomType = std::vector<uint8_t>;

  /**
   * @brief Structure representing a single information field
   */
  struct Field {
    std::string attr;
    genie::core::DataType attr_type;
    std::vector<CustomType> attr_values;
  };

  /**
   * @brief Reads information fields from a bit reader
   * @param reader The bit reader to read from
   * @return
   */
  void Read(genie::util::BitReader& reader) {
    auto attr_count = static_cast<uint8_t>(reader.ReadBits(8));
    fields_.reserve(attr_count * 16);
    for (auto i = 0; i < attr_count; ++i) {
      auto attr_len = static_cast<uint8_t>(reader.ReadBits(8));
      std::string attr(attr_len, 0);
      for (auto& ch : attr)
        ch = reader.ReadAlignedInt<uint8_t>();
      auto attr_type = static_cast<genie::core::DataType>(reader.ReadAlignedInt<uint8_t>());
      auto attr_array_len = reader.ReadAlignedInt<uint8_t>();
      ArrayType arrayType;
      std::vector<CustomType> values;
      for (auto j = 0; j < attr_array_len; ++j) {
        std::vector<uint8_t> value;
        if (attr_type == DataType::CHAR) {
          uint8_t len = reader.ReadAlignedInt<uint8_t>();
          value = std::vector<uint8_t>(len);
          for (auto& ch : value) {
            auto chVec = arrayType.toArray(attr_type, reader);
            ch = chVec[0];
          }
        } else {
          value = arrayType.toArray(attr_type, reader);
        }
            
        values.push_back(value);
      }
      if (attr_type == DataType::CHAR) {
        attr_type = DataType::STRING;
      }
      Field field{attr, attr_type, values};
      fields_.push_back(field);
    }
  }

  /**
   * @brief Clears all information fields
   */
  void Clear() {
    fields_.clear();
    fields_.shrink_to_fit();
  }
  /**
   * @brief Gets the vector of information fields
   * @return Reference to the vector of fields
   */
  std::vector<Field>& GetFields() {
    return fields_;
  }

 private:
  std::vector<Field> fields_;  //!< @brief Container for all information fields
};

/**
 * @brief Structure representing a tag in the info field
 */
struct Info_tag {
  uint8_t info_tag_len;
  std::string info_tag;
  genie::core::DataType info_type;
  uint8_t info_array_len;
  std::vector<std::vector<uint8_t>> infoValue;
};

/**
 * @brief Structure representing an alternative allele
 */
struct AlternativeAllele {
  std::string alt;
};

/**
 * @brief Class representing a feature record
 */
class Record {
 private:
  uint64_t feature_index_;   //!< @brief Index of the feature
  FeatureFields feature_attributes_;  //!< @brief Feature attributes

  uint8_t linked_record_;     //!< @brief Flag indicating if record is linked
  uint8_t link_name_len_;     //!< @brief Length of the link name
  std::string link_name_;     //!< @brief Link name string
  uint8_t reference_box_id_;  //!< @brief Reference box identifier

  /**
   * @brief Clears all data in the record
   */
  void ClearData();

 public:
  /**
   * @brief Default constructor for Record
   */
  Record()
      : feature_index_(0),
        feature_attributes_(),
        linked_record_(0),
        link_name_len_(0),
        link_name_(""),
        reference_box_id_(0) {}

  /**
   * @brief Parameterized constructor for Record
   * @param feature_index Index of the feature
   * @param feature_attributes Feature attributes
   * @param linked_record Flag indicating if record is linked
   * @param link_name_len Length of the link name
   * @param link_name Link name string
   * @param reference_box_ID Reference box identifier
   */
  Record(uint64_t feature_index, FeatureFields feature_attributes, uint8_t linked_record,
         uint8_t link_name_len, std::string link_name, uint8_t reference_box_ID)
      : feature_index_(feature_index),
        feature_attributes_(feature_attributes),
        linked_record_(linked_record),
        link_name_len_(link_name_len),
        link_name_(link_name),
        reference_box_id_(reference_box_ID) {}
  /**
   * @brief Constructor from a bit reader
   * @param reader The bit reader to read from
   */
  explicit Record(genie::util::BitReader& reader);
  /**
   * @brief
   */
  //  Record(genie::util::BitReader& reader, std::vector<Info_tag> infoTag) :
  //  info_tag(infoTag) { read(reader); }
  //   Record(genie::util::BitReader& reader) { read(reader); }

  /**
   * @brief Reads record data from a bit reader
   * @param reader The bit reader to read from
   * @return True if read was successful, false otherwise
   */
  bool Read(genie::util::BitReader& reader);

  /**
   * @brief Writes record data to a writer
   * @param writer The writer to write to
   */
  void Write(genie::util::BitWriter& writer);

  /**
   * @brief Gets the feature index
   * @return The feature index
   */
  [[nodiscard]] uint64_t GetFeatureIndex() const {
    return feature_index_;
  }

  /**
   * @brief Gets the feature attributes
   * @return Reference to the feature attributes
   */
  FeatureFields& GetFeatureAttributes() {
    return feature_attributes_;
  }

  /**
   * @brief Checks if this is a linked record
   * @return True if this is a linked record, false otherwise
   */
  [[nodiscard]] bool IsLinkedRecord() const {
    return (linked_record_ == 0 ? false : true);
  }

  /**
   * @brief Gets the link name
   * @return The link name string
   */
  [[nodiscard]] std::string GetLinkName() const {
    return link_name_;
  }

  /**
   * @brief Gets the reference box ID
   * @return The reference box ID
   */
  [[nodiscard]] uint8_t GetReferenceBoxID() const {
    return reference_box_id_;
  }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::feature

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_FEATURE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

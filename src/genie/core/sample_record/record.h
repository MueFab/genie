/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_SAMPLE_RECORD_RECORD_H_
#define SRC_GENIE_CORE_SAMPLE_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_writer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::sample {

/**
 * @brief Class for handling information fields in sample records
 */
class SampleFields {
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
    DataType attr_type;
    std::vector<CustomType> attr_values;
  };

  /**
   * @brief Reads information fields from a bit reader
   * @param reader The bit reader to read from
   * @return
   */
  void Read(util::BitReader& reader) {
    auto attr_count = static_cast<uint8_t>(reader.ReadBits(8));
    fields_.reserve(attr_count * 16);
    for (auto i = 0; i < attr_count; ++i) {
      auto attr_len = static_cast<uint8_t>(reader.ReadBits(8));
      std::string attr(attr_len, 0);
      for (auto& ch : attr)
        ch = reader.ReadAlignedInt<uint8_t>();
      auto attr_type = static_cast<DataType>(reader.ReadAlignedInt<uint8_t>());
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
  DataType info_type;
  uint8_t info_array_len;
  std::vector<std::vector<uint8_t>> infoValue;
};

/**
 * @brief Class representing a sample record
 */
class Record {
 private:
  uint64_t sample_index_;   //!< @brief Index of the sample
  SampleFields sample_attributes_;  //!< @brief Sample attributes

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
      : sample_index_(0),
        sample_attributes_(),
        linked_record_(0),
        link_name_len_(0),
        link_name_(""),
        reference_box_id_(0) {}

  /**
   * @brief Constructor from a bit reader
   * @param reader The bit reader to read from
   */
  explicit Record(util::BitReader& reader);

  /**
   * @brief Reads record data from a bit reader
   * @param reader The bit reader to read from
   * @return True if read was successful, false otherwise
   */
  bool Read(util::BitReader& reader);

  /**
   * @brief Writes record data to a writer
   * @param writer The writer to write to
   */
  void Write(util::BitWriter& writer);

  /**
   * @brief Gets the feature index
   * @return The sample index
   */
  [[nodiscard]] uint64_t GetSampleIndex() const {
    return sample_index_;
  }

  /**
   * @brief Gets the sample attributes
   * @return Reference to the sample attributes
   */
  SampleFields& GetSampleAttributes() {
    return sample_attributes_;
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

}  // namespace genie::core::record::sample

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_SAMPLE_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

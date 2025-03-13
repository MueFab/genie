/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_SITE_RECORD_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_SITE_RECORD_H_

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

namespace genie::core::record::variant_site {

/**
 * @brief Class for handling information fields in variant site records
 */
class InfoFields {
 public:
  /**
   * @brief Custom type definition for field values
   */
  using CustomType = std::vector<uint8_t>;

  /**
   * @brief Structure representing a single information field
   */
  struct Field {
    std::string tag;
    genie::core::DataType type;
    std::vector<CustomType> values;
  };

  /**
   * @brief Reads information fields from a bit reader
   * @param reader The bit reader to read from
   * @return
   */
  void Read(genie::util::BitReader& reader) {
    auto alt_count = static_cast<uint8_t>(reader.ReadBits(8));
    fields_.reserve(alt_count * 16);
    for (auto i = 0; i < alt_count; ++i) {
      auto alt_len = static_cast<uint8_t>(reader.ReadBits(8));
      std::string alttag(alt_len, 0);
      for (auto& tag : alttag)
        tag = reader.ReadAlignedInt<uint8_t>();
      auto alt_type = static_cast<genie::core::DataType>(reader.ReadAlignedInt<uint8_t>());
      auto arrayLength = reader.ReadAlignedInt<uint8_t>();
      ArrayType arrayType;
      std::vector<CustomType> values;
      for (auto j = 0; j < arrayLength; ++j) {
        std::vector<uint8_t> value = arrayType.toArray(alt_type, reader);
        values.push_back(value);
      }

      Field field{alttag, alt_type, values};
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
 * @brief Class representing a variant site record
 */
class Record {
 private:
  uint64_t variant_index_;   //!< @brief Index of the variant
  uint16_t seq_id_;          //!< @brief Sequence identifier
  uint64_t pos_;             //!< @brief Position in the sequence
  uint8_t strand_;           //!< @brief Strand information
  uint8_t id_len_;           //!< @brief Length of the ID string
  std::string id_;           //!< @brief ID string
  uint8_t description_len_;  //!< @brief Length of the description
  std::string description_;  //!< @brief Description string
  uint32_t ref_len_;         //!< @brief Length of the reference string
  std::string ref_;          //!< @brief Reference string
  uint8_t alt_count_;        //!< @brief Count of alternative alleles

  std::vector<uint32_t> alt_len_;    //!< @brief Lengths of alternative alleles
  std::vector<std::string> altern_;  //!< @brief Alternative allele strings

  uint32_t depth_;           //!< @brief Sequencing depth
  uint32_t seq_qual_;        //!< @brief Sequence quality
  uint32_t map_qual_;        //!< @brief Mapping quality
  uint32_t map_num_qual_0_;  //!< @brief Number of reads with mapping quality 0
  uint8_t filters_len_;      //!< @brief Length of the filters string
  std::string filters_;      //!< @brief Filters string

  InfoFields info_;  //!< @brief Information fields

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
      : variant_index_(0),
        seq_id_(0),
        pos_(0),
        strand_(0),
        id_len_(0),
        id_(""),
        description_len_(0),
        description_(""),
        ref_len_(0),
        ref_(""),
        alt_count_(0),
        alt_len_(0),
        altern_(0),
        depth_(0),
        seq_qual_(0),
        map_qual_(0),
        map_num_qual_0_(0),
        filters_len_(0),
        filters_(""),
        linked_record_(0),
        link_name_len_(0),
        link_name_(""),
        reference_box_id_(0) {}

  /**
   * @brief Parameterized constructor for Record
   * @param variant_index Index of the variant
   * @param seq_id Sequence identifier
   * @param pos Position in the sequence
   * @param strand Strand information
   * @param id_len Length of the ID string
   * @param id ID string
   * @param description_len Length of the description
   * @param description Description string
   * @param ref_len Length of the reference string
   * @param ref Reference string
   * @param alt_count Count of alternative alleles
   * @param alt_len Lengths of alternative alleles
   * @param altern Alternative allele strings
   * @param depth Sequencing depth
   * @param seq_qual Sequence quality
   * @param map_qual Mapping quality
   * @param map_num_qual_0 Number of reads with mapping quality 0
   * @param filters_len Length of the filters string
   * @param filters Filters string
   * @param linked_record Flag indicating if record is linked
   * @param link_name_len Length of the link name
   * @param link_name Link name string
   * @param reference_box_ID Reference box identifier
   */
  Record(uint64_t variant_index, uint16_t seq_id, uint64_t pos, uint8_t strand, uint8_t id_len,
         std::string& id, uint8_t description_len, std::string description, uint32_t ref_len,
         std::string ref, uint8_t alt_count, std::vector<uint32_t> alt_len,
         std::vector<std::string> altern, uint32_t depth, uint32_t seq_qual, uint32_t map_qual,
         uint32_t map_num_qual_0, uint8_t filters_len, std::string filters, uint8_t linked_record,
         uint8_t link_name_len, std::string link_name, uint8_t reference_box_ID)
      : variant_index_(variant_index),
        seq_id_(seq_id),
        pos_(pos),
        strand_(strand),
        id_len_(id_len),
        id_(id),
        description_len_(description_len),
        description_(description),
        ref_len_(ref_len),
        ref_(ref),
        alt_count_(alt_count),
        alt_len_(alt_len),
        altern_(altern),
        depth_(depth),
        seq_qual_(seq_qual),
        map_qual_(map_qual),
        map_num_qual_0_(map_num_qual_0),
        filters_len_(filters_len),
        filters_(filters),
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
   * @brief Gets the variant index
   * @return The variant index
   */
  [[nodiscard]] uint64_t GetVariantIndex() const {
    return variant_index_;
  }

  /**
   * @brief Gets the sequence ID
   * @return The sequence ID
   */
  [[nodiscard]] uint16_t GetSeqId() const {
    return seq_id_;
  }

  /**
   * @brief Gets the position
   * @return The position in the sequence
   */
  [[nodiscard]] uint64_t GetPos() const {
    return pos_;
  }

  /**
   * @brief Gets the strand information
   * @return The strand value
   */
  [[nodiscard]] uint8_t GetStrand() const {
    return strand_;
  }

  /**
   * @brief Gets the ID string
   * @return The ID string
   */
  [[nodiscard]] std::string GetId() const {
    return id_;
  }

  /**
   * @brief Gets the description
   * @return The description string
   */
  [[nodiscard]] std::string GetDescription() const {
    return description_;
  }

  /**
   * @brief Gets the reference string
   * @return The reference string
   */
  [[nodiscard]] std::string GetRef() const {
    return ref_;
  }

  /**
   * @brief Gets the count of alternative alleles
   * @return The count of alternative alleles
   */
  [[nodiscard]] uint8_t GetAltCount() const {
    return alt_count_;
  }

  /**
   * @brief Gets the alternative alleles
   * @return Vector of alternative allele strings
   */
  [[nodiscard]] std::vector<std::string> GetAlt() const {
    return altern_;
  }

  /**
   * @brief Gets the sequencing depth
   * @return The sequencing depth
   */
  [[nodiscard]] uint32_t GetDepth() const {
    return depth_;
  }

  /**
   * @brief Gets the sequence quality
   * @return The sequence quality
   */
  [[nodiscard]] uint32_t GetSeqQual() const {
    return seq_qual_;
  }

  /**
   * @brief Gets the mapping quality
   * @return The mapping quality
   */
  [[nodiscard]] uint32_t GetMapQual() const {
    return map_qual_;
  }

  /**
   * @brief Gets the number of reads with mapping quality 0
   * @return The number of reads with mapping quality 0
   */
  [[nodiscard]] uint32_t GetMapNumQual0() const {
    return map_num_qual_0_;
  }

  /**
   * @brief Gets the filters string
   * @return The filters string
   */
  [[nodiscard]] std::string GetFilters() const {
    return filters_;
  }

  /**
   * @brief Gets the information field tags
   * @return Reference to the vector of information field tags
   */
  std::vector<InfoFields::Field>& GetInfoTag() {
    return info_.GetFields();
  }
  //  std::vector<std::vector<std::vector<uint8_t>>> getInfoValues() const {
  //  return infoValue; }

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

}  // namespace genie::core::record::variant_site

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_SITE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

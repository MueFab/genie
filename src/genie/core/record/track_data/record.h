/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_TRACK_DATA_RECORD_H_
#define SRC_GENIE_CORE_RECORD_TRACK_DATA_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/core/access_unit/annotation/attribute_field.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::track_data {

/**
 * @brief Structure representing an attribute entry
 */
struct Attribute {
    uint8_t attr_tag_len;
    std::string attr_tag;
    uint8_t attr_type;
    std::vector<std::vector<uint8_t>> attr_values;
};

/**
 * @brief Class representing a track data record
 */
class Record {
 private:
    uint64_t track_data_index_;  //!< @brief Index of the track data
    uint16_t seq_id_;            //!< @brief Sequence identifier
    uint64_t start_pos_;         //!< @brief Start position (40-bit value)
    uint64_t end_pos_;           //!< @brief End position (40-bit value)
    uint8_t strand_;             //!< @brief Strand information
    uint8_t attr_count_;         //!< @brief Count of attribute entries

    std::vector<Attribute> attributes_;  //!< @brief Vector of attribute entries

    uint8_t reserved_;          //!< @brief Reserved bits (7 bits)
    uint8_t linked_record_;     //!< @brief Flag indicating if record is linked (1 bit)
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
        : track_data_index_(0),
          seq_id_(0),
          start_pos_(0),
          end_pos_(0),
          strand_(0),
          attr_count_(0),
          attributes_(0),
          reserved_(0),
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

    // Getter methods

    /**
     * @brief Gets the track data index
     * @return The track data index
     */
    [[nodiscard]] uint64_t GetTrackDataIndex() const {
        return track_data_index_;
    }

    /**
     * @brief Gets the sequence ID
     * @return The sequence ID
     */
    [[nodiscard]] uint16_t GetSeqId() const {
        return seq_id_;
    }

    /**
     * @brief Gets the start position
     * @return The start position
     */
    [[nodiscard]] uint64_t GetStartPos() const {
        return start_pos_;
    }

    /**
     * @brief Gets the end position
     * @return The end position
     */
    [[nodiscard]] uint64_t GetEndPos() const {
        return end_pos_;
    }

    /**
     * @brief Gets the strand information
     * @return The strand value
     */
    [[nodiscard]] uint8_t GetStrand() const {
        return strand_;
    }

    /**
     * @brief Gets the attribute count
     * @return The attribute count
     */
    [[nodiscard]] uint8_t GetAttrCount() const {
        return attr_count_;
    }

    /**
     * @brief Gets the attributes
     * @return Vector of attribute entries
     */
    [[nodiscard]] const std::vector<Attribute>& GetAttributes() const {
        return attributes_;
    }

    /**
     * @brief Gets the attributes as AttributeField vector
     * @return Vector of AttributeField entries
     */
    [[nodiscard]] std::vector<genie::core::access_unit::annotation::AttributeField> GetFields() const {
        std::vector<genie::core::access_unit::annotation::AttributeField> fields;
        fields.reserve(attributes_.size());
        for (const auto& attr : attributes_) {
            fields.push_back({attr.attr_tag, static_cast<genie::core::DataType>(attr.attr_type), attr.attr_values});
        }
        return fields;
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

}  // namespace genie::core::record::track_data

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_TRACK_DATA_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

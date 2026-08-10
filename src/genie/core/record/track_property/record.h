/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_TRACK_PROPERTY_RECORD_H_
#define SRC_GENIE_CORE_RECORD_TRACK_PROPERTY_RECORD_H_

#include <string>
#include <vector>

#include "genie/core/access_unit/annotation/attribute_field.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

namespace genie::core::record::track_property {

struct TrackProperty {
    uint8_t track_property_len;
    std::string track_property;
    uint8_t track_property_type;
    uint8_t track_property_array_len;
    std::vector<std::vector<uint8_t>> track_property_values;
};

class Record {
 private:
    uint16_t track_index_;
    uint8_t track_type_;
    uint8_t track_property_count_;
    std::vector<TrackProperty> properties_;
    uint8_t reserved_;
    uint8_t linked_record_;
    uint8_t link_name_len_;
    std::string link_name_;
    uint8_t reference_box_id_;

    void ClearData();

 public:
    Record() = default;
    explicit Record(util::BitReader& reader);

    void Write(util::BitWriter& writer);
    bool Read(util::BitReader& reader);

    // Getters
    [[nodiscard]] uint16_t GetTrackIndex() const { return track_index_; }
    [[nodiscard]] uint8_t GetTrackType() const { return track_type_; }
    [[nodiscard]] uint8_t GetTrackPropertyCount() const { return track_property_count_; }
    [[nodiscard]] const std::vector<TrackProperty>& GetProperties() const { return properties_; }
    [[nodiscard]] std::vector<genie::core::access_unit::annotation::AttributeField> GetFields() const {
        std::vector<genie::core::access_unit::annotation::AttributeField> fields;
        fields.reserve(properties_.size());
        for (const auto& prop : properties_) {
            fields.push_back({prop.track_property, static_cast<genie::core::DataType>(prop.track_property_type),
                             prop.track_property_values});
        }
        return fields;
    }
    [[nodiscard]] uint8_t GetReserved() const { return reserved_; }
    [[nodiscard]] uint8_t GetLinkedRecord() const { return linked_record_; }
    [[nodiscard]] bool IsLinkedRecord() const {
      return (linked_record_ == 0 ? false : true);
    }
    [[nodiscard]] uint8_t GetLinkNameLen() const { return link_name_len_; }
    [[nodiscard]] const std::string& GetLinkName() const { return link_name_; }
    [[nodiscard]] uint8_t GetReferenceBoxID() const { return reference_box_id_; }

    // Setters (if needed)
    void SetTrackIndex(uint16_t index) { track_index_ = index; }
    void SetTrackType(uint8_t type) { track_type_ = type; }
};

}  // namespace genie::core::record::track_property

#endif  // SRC_GENIE_CORE_RECORD_TRACK_PROPERTY_RECORD_H_

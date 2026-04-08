/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/track_property_record/record.h"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// -------------------------------------------------------------------------------------------------

namespace genie::core::record::track_property {

// -------------------------------------------------------------------------------------------------

Record::Record(util::BitReader& reader) {
    Read(reader);
}

// -------------------------------------------------------------------------------------------------

void Record::Write(util::BitWriter& writer) {
    writer.WriteBits(track_index_, 16);
    writer.WriteBits(track_type_, 8);
    writer.WriteBits(track_property_count_, 8);

    ArrayType writeType;
    for (auto i = 0; i < track_property_count_; ++i) {
        writer.WriteBits(properties_[i].track_property_len, 8);
        writer.Write(properties_[i].track_property);
        writer.WriteBits(properties_[i].track_property_type, 8);
        writer.WriteBits(properties_[i].track_property_array_len, 8);

        DataType type = static_cast<DataType>(properties_[i].track_property_type);
        writeType.toFile(type, properties_[i].track_property_values, writer);
    }

    writer.WriteBits(reserved_, 7);
    writer.WriteBits(linked_record_, 1);
    if (linked_record_) {
        writer.WriteBits(link_name_len_, 8);
        writer.Write(link_name_);
        writer.WriteBits(reference_box_id_, 8);
    }
}

// -------------------------------------------------------------------------------------------------

bool Record::Read(util::BitReader& reader) {
    ClearData();
    track_index_ = static_cast<uint16_t>(reader.ReadBits(16));
    if (!reader.IsStreamGood())
        return false;

    track_type_ = static_cast<uint8_t>(reader.ReadBits(8));
    track_property_count_ = static_cast<uint8_t>(reader.ReadBits(8));

    properties_.clear();
    properties_.reserve(track_property_count_);
    ArrayType readType;

    for (auto i = 0; i < track_property_count_; ++i) {
        TrackProperty prop;
        prop.track_property_len = static_cast<uint8_t>(reader.ReadBits(8));
        if (prop.track_property_len > 0) {
            prop.track_property.resize(prop.track_property_len);
            reader.ReadAlignedBytes(&prop.track_property[0], prop.track_property_len);
        }
        prop.track_property_type = static_cast<uint8_t>(reader.ReadBits(8));
        prop.track_property_array_len = static_cast<uint8_t>(reader.ReadBits(8));

        DataType type = static_cast<DataType>(prop.track_property_type);
        prop.track_property_values = readType.toArray(type, reader);
        properties_.push_back(prop);
    }

    uint8_t reserved_and_linked = static_cast<uint8_t>(reader.ReadBits(8));
    reserved_ = (reserved_and_linked >> 1) & 0x7F;
    linked_record_ = reserved_and_linked & 0x01;

    if (linked_record_) {
        link_name_len_ = static_cast<uint8_t>(reader.ReadBits(8));
        if (link_name_len_ > 0) {
            link_name_.resize(link_name_len_);
            reader.ReadAlignedBytes(&link_name_[0], link_name_len_);
        }
        reference_box_id_ = static_cast<uint8_t>(reader.ReadBits(8));
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

void Record::ClearData() {
    track_index_ = 0;
    track_type_ = 0;
    track_property_count_ = 0;
    properties_.clear();
    reserved_ = 0;
    linked_record_ = 0;
    link_name_len_ = 0;
    link_name_.clear();
    reference_box_id_ = 0;
}

// -------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::track_property

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

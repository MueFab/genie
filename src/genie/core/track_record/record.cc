/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/track_record/record.h"

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

namespace genie::core::record::track {

// -------------------------------------------------------------------------------------------------

Record::Record(util::BitReader& reader) {
    Read(reader);
}

// -------------------------------------------------------------------------------------------------

void Record::Write(util::BitWriter& writer) {
    writer.WriteBits(track_data_index_, 64);
    writer.WriteBits(seq_id_, 16);
    writer.WriteBits(start_pos_, 40);
    writer.WriteBits(end_pos_, 40);
    writer.WriteBits(strand_, 8);
    writer.WriteBits(attr_count_, 8);

    ArrayType writeType;
    for (auto i = 0; i < attr_count_; ++i) {
        writer.WriteBits(attributes_[i].attr_tag_len, 8);
        writer.Write(attributes_[i].attr_tag);
        writer.WriteBits(attributes_[i].attr_type, 8);
        DataType type = static_cast<DataType>(attributes_[i].attr_type);
        writeType.toFile(type, attributes_[i].attr_value, writer);
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
    track_data_index_ = reader.ReadBits(64);
    if (!reader.IsStreamGood())
        return false;

    seq_id_ = static_cast<uint16_t>(reader.ReadBits(16));
    start_pos_ = reader.ReadBits(40);
    end_pos_ = reader.ReadBits(40);
    strand_ = static_cast<uint8_t>(reader.ReadBits(8));
    attr_count_ = static_cast<uint8_t>(reader.ReadBits(8));

    attributes_.clear();
    attributes_.reserve(attr_count_);
    ArrayType readType;
    for (auto i = 0; i < attr_count_; ++i) {
        Attribute attr;
        attr.attr_tag_len = static_cast<uint8_t>(reader.ReadBits(8));
        if (attr.attr_tag_len > 0) {
            attr.attr_tag.resize(attr.attr_tag_len);
            reader.ReadAlignedBytes(&attr.attr_tag[0], attr.attr_tag_len);
        }
        attr.attr_type = static_cast<uint8_t>(reader.ReadBits(8));
        DataType type = static_cast<DataType>(attr.attr_type);
        attr.attr_value = readType.toArray(type, reader);
        attributes_.push_back(attr);
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
    track_data_index_ = 0;
    seq_id_ = 0;
    start_pos_ = 0;
    end_pos_ = 0;
    strand_ = 0;
    attr_count_ = 0;
    attributes_.clear();
    reserved_ = 0;
    linked_record_ = 0;
    link_name_len_ = 0;
    link_name_.clear();
    reference_box_id_ = 0;
}

// -------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::track

// -------------------------------------------------------------------------------------------------

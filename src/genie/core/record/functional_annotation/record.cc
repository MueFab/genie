/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/functional_annotation/record.h"

#include <vector>

#include "genie/core/array_type.h"

// -------------------------------------------------------------------------------------------------

namespace genie::core::record::functional_annotation {

// -------------------------------------------------------------------------------------------------

Record::Record(util::BitReader& reader) {
    Read(reader);
}

// -------------------------------------------------------------------------------------------------

void Record::Write(util::BitWriter& writer) {
    writer.WriteBits(annotation_index_, 64);
    writer.WriteBits(seq_id_, 16);
    writer.WriteBits(start_pos_, 40);
    writer.WriteBits(end_pos_, 40);
    writer.WriteBits(feature_name_len_, 8);
    writer.WriteAlignedBytes(feature_name_.data(), feature_name_.size());
    writer.WriteBits(feature_ID_, 24);
    writer.WriteBits(ontology_count_, 8);

    for (auto i = 0; i < ontology_count_; ++i) {
        writer.WriteBits(ontologies_[i].ontology_name_len, 8);
        writer.WriteAlignedBytes(ontologies_[i].ontology_name.data(), ontologies_[i].ontology_name.size());
        writer.WriteBits(ontologies_[i].ontology_ID, 24);
    }

    writer.WriteBits(strand_, 8);
    writer.WriteBits(attr_count_, 8);

    ArrayType writeType;
    for (auto i = 0; i < attr_count_; ++i) {
        writer.WriteBits(attributes_[i].attr_tag_len, 8);
        writer.WriteAlignedBytes(attributes_[i].attr_tag.data(), attributes_[i].attr_tag.size());
        writer.WriteBits(attributes_[i].attr_type, 8);
        writer.WriteBits(attributes_[i].attr_array_len, 8);
        DataType type = static_cast<DataType>(attributes_[i].attr_type);
        for (auto j = 0; j < attributes_[i].attr_array_len; ++j) {
            writeType.toFile(type, attributes_[i].attr_values[j], writer);
        }
    }

    writer.WriteBits(reserved_, 7);
    writer.WriteBits(linked_record_, 1);
    if (linked_record_) {
        writer.WriteBits(link_name_len_, 8);
        writer.WriteAlignedBytes(link_name_.data(), link_name_.size());
        writer.WriteBits(reference_box_id_, 8);
    }
}

// -------------------------------------------------------------------------------------------------

bool Record::Read(util::BitReader& reader) {
    ClearData();
    annotation_index_ = reader.ReadBits(64);
    if (!reader.IsStreamGood())
        return false;

    seq_id_ = static_cast<uint16_t>(reader.ReadBits(16));
    start_pos_ = reader.ReadBits(40);
    end_pos_ = reader.ReadBits(40);
    feature_name_len_ = static_cast<uint8_t>(reader.ReadBits(8));

    if (feature_name_len_ > 0) {
        feature_name_.resize(feature_name_len_);
        reader.ReadAlignedBytes(&feature_name_[0], feature_name_len_);
    }

    feature_ID_ = static_cast<uint32_t>(reader.ReadBits(24));
    ontology_count_ = static_cast<uint8_t>(reader.ReadBits(8));

    ontologies_.clear();
    ontologies_.reserve(ontology_count_);
    for (auto i = 0; i < ontology_count_; ++i) {
        Ontology onto;
        onto.ontology_name_len = static_cast<uint8_t>(reader.ReadBits(8));
        if (onto.ontology_name_len > 0) {
            onto.ontology_name.resize(onto.ontology_name_len);
            reader.ReadAlignedBytes(&onto.ontology_name[0], onto.ontology_name_len);
        }
        onto.ontology_ID = static_cast<uint32_t>(reader.ReadBits(24));
        ontologies_.push_back(onto);
    }

    strand_ = static_cast<uint8_t>(reader.ReadBits(8));
    attr_count_ = static_cast<uint8_t>(reader.ReadBits(8));

    attributes_.clear();
    attributes_.reserve(attr_count_);
    ArrayType arrayType;
    for (auto i = 0; i < attr_count_; ++i) {
        Attribute attr;
        attr.attr_tag_len = static_cast<uint8_t>(reader.ReadBits(8));
        if (attr.attr_tag_len > 0) {
            attr.attr_tag.resize(attr.attr_tag_len);
            reader.ReadAlignedBytes(&attr.attr_tag[0], attr.attr_tag_len);
        }
        attr.attr_type = static_cast<uint8_t>(reader.ReadBits(8));
        attr.attr_array_len = static_cast<uint8_t>(reader.ReadBits(8));
        DataType type = static_cast<DataType>(attr.attr_type);
        attr.attr_values.clear();
        attr.attr_values.reserve(attr.attr_array_len);
        for (auto j = 0; j < attr.attr_array_len; ++j) {
            std::vector<uint8_t> value = arrayType.toArray(type, reader);
            attr.attr_values.push_back(value);
        }
        attributes_.push_back(attr);
    }

    uint8_t combined = static_cast<uint8_t>(reader.ReadBits(8));
    reserved_ = (combined >> 1) & 0x7F;
    linked_record_ = combined & 0x01;

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
    annotation_index_ = 0;
    seq_id_ = 0;
    start_pos_ = 0;
    end_pos_ = 0;
    feature_name_len_ = 0;
    feature_name_ = "";
    feature_ID_ = 0;
    ontology_count_ = 0;
    ontologies_.clear();
    strand_ = 0;
    attr_count_ = 0;
    attributes_.clear();
    reserved_ = 0;
    linked_record_ = 0;
    link_name_len_ = 0;
    link_name_ = "";
    reference_box_id_ = 0;
}

// -------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::functional_annotation

// -------------------------------------------------------------------------------------------------

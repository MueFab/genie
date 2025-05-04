/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/parameter_set.h"

#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "genie/core/global_cfg.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

// -----------------------------------------------------------------------------

bool EncodingSet::SignatureCfg::operator==(const SignatureCfg& cfg) const {
  return signature_length == cfg.signature_length;
}

// -----------------------------------------------------------------------------

EncodingSet::EncodingSet(util::BitReader& bit_reader) {
  dataset_type_ = bit_reader.Read<DataUnit::DatasetType>(4);
  alphabet_id_ = bit_reader.Read<AlphabetId>();
  read_length_ = bit_reader.Read<uint32_t>(24);
  number_of_template_segments_minus1_ = bit_reader.Read<uint8_t>(2);
  bit_reader.ReadBits(6);
  max_au_data_unit_size_ = bit_reader.Read<uint32_t>(29);
  pos_40_bits_flag_ = bit_reader.Read<bool>(1);
  qv_depth_ = bit_reader.Read<uint8_t>(3);
  as_depth_ = bit_reader.Read<uint8_t>(3);
  auto num_classes = bit_reader.Read<uint8_t>(4);
  for (size_t i = 0; i < num_classes; ++i) {
    class_i_ds_.push_back(bit_reader.Read<record::ClassType>(4));
  }
  for (size_t i = 0; i < GetDescriptors().size(); ++i) {
    descriptors_.emplace_back(num_classes, static_cast<GenDesc>(i), bit_reader);
  }
  const auto num_groups = bit_reader.Read<uint16_t>();
  for (size_t i = 0; i < num_groups; ++i) {
    read_group_index_ds_.emplace_back();
    char c = 0;
    do {
      c = static_cast<char>(bit_reader.Read<uint8_t>());
      read_group_index_ds_.back().push_back(c);
    } while (c);
  }
  multiple_alignments_flag_ = bit_reader.Read<bool>(1);
  spliced_reads_flag_ = bit_reader.Read<bool>(1);
  extended_alignment_flag_ = bit_reader.Read<bool>(1);
  reserved_ = bit_reader.Read<uint32_t>(29);
  if (bit_reader.Read<bool>(1)) {
    signature_cfg_ = SignatureCfg();
    if (bit_reader.Read<bool>(1)) {
      signature_cfg_->signature_length = bit_reader.Read<uint8_t>(8);
    }
  }
  for (size_t i = 0; i < num_classes; ++i) {
    if (const auto mode = bit_reader.Read<uint8_t>(4); mode == 1) {
      qv_coding_configs_.emplace_back(
          GlobalCfg::GetSingleton()
              .GetIndustrialPark()
              .Construct<QualityValues>(mode, GenDesc::kQv, bit_reader));
    } else {
      bit_reader.Read<uint8_t>(1);
    }
  }
  if (bit_reader.Read<bool>(1)) {
    computed_reference_ = ComputedRef(bit_reader);
  }
  bit_reader.FlushHeldBits();
}

//------------------------------------------------------------------------------

EncodingSet::EncodingSet(const DataUnit::DatasetType dataset_type,
                         const AlphabetId alphabet_id,
                         const uint32_t read_length, const bool paired_end,
                         const bool pos_40_bits_flag, const uint8_t qv_depth,
                         const uint8_t as_depth,
                         const bool multiple_alignments_flag,
                         const bool spliced_reads_flag,
                         const bool extended_alignment_flag)
    : dataset_type_(dataset_type),
      alphabet_id_(alphabet_id),
      read_length_(read_length),
      number_of_template_segments_minus1_(static_cast<uint8_t>(paired_end)),
      max_au_data_unit_size_(0),
      pos_40_bits_flag_(pos_40_bits_flag),
      qv_depth_(qv_depth),
      as_depth_(as_depth),
      class_i_ds_(0),
      descriptors_(18),
      read_group_index_ds_(0),
      multiple_alignments_flag_(multiple_alignments_flag),
      spliced_reads_flag_(spliced_reads_flag),
      extended_alignment_flag_(extended_alignment_flag),
      qv_coding_configs_(0) {}

// -----------------------------------------------------------------------------

EncodingSet::EncodingSet()
    : dataset_type_(DataUnit::DatasetType::kAligned),
      alphabet_id_(AlphabetId::kAcgtn),
      read_length_(0),
      number_of_template_segments_minus1_(0),
      max_au_data_unit_size_(0),
      pos_40_bits_flag_(false),
      qv_depth_(0),
      as_depth_(0),
      class_i_ds_(0),
      descriptors_(18),
      read_group_index_ds_(0),
      multiple_alignments_flag_(false),
      spliced_reads_flag_(false),
      extended_alignment_flag_(false),
      qv_coding_configs_(0) {}

// -----------------------------------------------------------------------------

void ParameterSet::Write(util::BitWriter& writer) const {
  DataUnit::Write(writer);
  writer.WriteBits(0, 10);  // reserved

  // Calculate Size and write structure to tmp buffer
  std::stringstream ss;
  util::BitWriter tmp_writer(ss);
  tmp_writer.WriteBits(parameter_set_id_, 8);
  tmp_writer.WriteBits(parent_parameter_set_id_, 8);
  set_.Write(tmp_writer);
  tmp_writer.FlushBits();
  uint64_t bits = tmp_writer.GetTotalBitsWritten();
  constexpr uint64_t type_size_size =
      8 + 10 + 22;  // data_unit_type, reserved, data_unit_size
  bits += type_size_size;
  const uint64_t bytes = bits / 8 + (bits % 8 ? 1 : 0);

  // Now Size is known, write to final destination
  writer.WriteBits(bytes, 22);
  writer.WriteAlignedStream(ss);
}

// -----------------------------------------------------------------------------

uint64_t ParameterSet::GetLength() const {
  std::stringstream ss;
  util::BitWriter tmp_writer(ss);
  tmp_writer.WriteBits(parameter_set_id_, 8);
  tmp_writer.WriteBits(parent_parameter_set_id_, 8);
  set_.Write(tmp_writer);

  const uint64_t len = tmp_writer.GetTotalBitsWritten() / 8;

  return len;
}

// -----------------------------------------------------------------------------

void EncodingSet::ActivateSignature() { signature_cfg_ = SignatureCfg(); }

// -----------------------------------------------------------------------------

bool EncodingSet::IsSignatureActivated() const {
  return signature_cfg_ != std::nullopt;
}

// -----------------------------------------------------------------------------

bool EncodingSet::IsSignatureConstLength() const {
  return IsSignatureActivated() &&
         signature_cfg_->signature_length != std::nullopt;
}

// -----------------------------------------------------------------------------

uint8_t EncodingSet::GetSignatureConstLength() const {
  return *signature_cfg_->signature_length;
}

// -----------------------------------------------------------------------------

void EncodingSet::SetSignatureLength(uint8_t length) {
  UTILS_DIE_IF(!IsSignatureActivated(), "Signature not activated");
  signature_cfg_->signature_length = length;
}

// -----------------------------------------------------------------------------

void EncodingSet::Write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(dataset_type_), 4);
  writer.WriteBits(static_cast<uint8_t>(alphabet_id_), 8);
  writer.WriteBits(read_length_, 24);
  writer.WriteBits(number_of_template_segments_minus1_, 2);
  writer.WriteBits(0, 6);  // reserved_2
  writer.WriteBits(max_au_data_unit_size_, 29);
  writer.WriteBits(pos_40_bits_flag_, 1);
  writer.WriteBits(qv_depth_, 3);
  writer.WriteBits(as_depth_, 3);
  writer.WriteBits(class_i_ds_.size(), 4);  // num_classes
  for (auto& i : class_i_ds_) {
    writer.WriteBits(static_cast<uint8_t>(i), 4);
  }
  for (auto& i : descriptors_) {
    i.Write(writer);
  }
  writer.WriteBits(read_group_index_ds_.size(), 16);  // num_groups
  for (auto& i : read_group_index_ds_) {
    for (auto& j : i) {
      writer.WriteBits(static_cast<uint8_t>(j), 8);
    }
    writer.WriteBits('\0', 8);  // NULL termination
  }
  writer.WriteBits(multiple_alignments_flag_, 1);
  writer.WriteBits(spliced_reads_flag_, 1);
  writer.WriteBits(extended_alignment_flag_, 1);
  writer.WriteBits(reserved_, 29);
  writer.WriteBits(signature_cfg_ != std::nullopt, 1);
  if (signature_cfg_ != std::nullopt) {
    writer.WriteBits(signature_cfg_->signature_length != std::nullopt, 1);
    if (signature_cfg_->signature_length != std::nullopt) {
      writer.WriteBits(*signature_cfg_->signature_length, 8);
    }
  }
  for (auto& i : qv_coding_configs_) {
    i->Write(writer);
  }
  writer.WriteBits(static_cast<bool>(computed_reference_), 1);
  if (computed_reference_) {
    computed_reference_->Write(writer);
  }
  writer.FlushBits();
}

// -----------------------------------------------------------------------------

[[nodiscard]] AlphabetId EncodingSet::GetAlphabetId() const {
  return alphabet_id_;
}

// -----------------------------------------------------------------------------

const ComputedRef& EncodingSet::GetComputedRef() const {
  return *computed_reference_;
}

// -----------------------------------------------------------------------------

size_t EncodingSet::GetNumberTemplateSegments() const {
  return number_of_template_segments_minus1_ + 1;
}

// -----------------------------------------------------------------------------

bool EncodingSet::IsComputedReference() const {
  return this->computed_reference_.has_value();
}

// -----------------------------------------------------------------------------

void EncodingSet::SetComputedRef(const ComputedRef& computed_reference) {
  computed_reference_ = computed_reference;
}

// -----------------------------------------------------------------------------

void EncodingSet::AddClass(const record::ClassType class_id,
                           std::unique_ptr<QualityValues> conf) {
  for (auto& a : descriptors_) {
    if (a.IsClassSpecific()) {
      UTILS_THROW_RUNTIME_EXCEPTION(
          "Adding classes not allowed once class specific descriptor "
          "configs enabled");
    }
  }
  for (const auto& a : class_i_ds_) {
    if (class_id == a) {
      UTILS_THROW_RUNTIME_EXCEPTION("Class already added");
    }
  }
  class_i_ds_.push_back(class_id);
  qv_coding_configs_.push_back(std::move(conf));
}

// -----------------------------------------------------------------------------

void EncodingSet::SetDescriptor(GenDesc index,
                                DescriptorSubSequenceCfg&& descriptor) {
  descriptors_[static_cast<uint8_t>(index)] = std::move(descriptor);
}

// -----------------------------------------------------------------------------

const DescriptorSubSequenceCfg& EncodingSet::GetDescriptor(
    GenDesc index) const {
  return descriptors_[static_cast<uint8_t>(index)];
}

// -----------------------------------------------------------------------------

void EncodingSet::AddGroup(std::string&& read_group_id) {
  read_group_index_ds_.emplace_back(std::move(read_group_id));
}

// -----------------------------------------------------------------------------

ParameterSet::DatasetType EncodingSet::GetDatasetType() const {
  return dataset_type_;
}

// -----------------------------------------------------------------------------

uint8_t EncodingSet::GetPosSize() const {
  return pos_40_bits_flag_ ? 40_u8 : 32_u8;
}

// -----------------------------------------------------------------------------

bool EncodingSet::HasMultipleAlignments() const {
  return multiple_alignments_flag_;
}

// -----------------------------------------------------------------------------

uint8_t ParameterSet::GetId() const { return parameter_set_id_; }

// -----------------------------------------------------------------------------

uint32_t EncodingSet::GetReadLength() const { return read_length_; }

// -----------------------------------------------------------------------------

EncodingSet& EncodingSet::operator=(const EncodingSet& other) {
  if (this == &other) {
    return *this;
  }
  dataset_type_ = other.dataset_type_;
  alphabet_id_ = other.alphabet_id_;
  read_length_ = other.read_length_;
  number_of_template_segments_minus1_ =
      other.number_of_template_segments_minus1_;
  max_au_data_unit_size_ = other.max_au_data_unit_size_;
  pos_40_bits_flag_ = other.pos_40_bits_flag_;
  qv_depth_ = other.qv_depth_;
  as_depth_ = other.as_depth_;
  class_i_ds_ = other.class_i_ds_;
  descriptors_ = other.descriptors_;
  read_group_index_ds_ = other.read_group_index_ds_;
  multiple_alignments_flag_ = other.multiple_alignments_flag_;
  spliced_reads_flag_ = other.spliced_reads_flag_;
  extended_alignment_flag_ = other.extended_alignment_flag_;
  signature_cfg_ = other.signature_cfg_;
  qv_coding_configs_.clear();
  for (const auto& c : other.qv_coding_configs_) {
    qv_coding_configs_.emplace_back(c->Clone());
  }
  computed_reference_ = other.computed_reference_;
  return *this;
}

// -----------------------------------------------------------------------------

EncodingSet& EncodingSet::operator=(EncodingSet&& other) noexcept {
  dataset_type_ = other.dataset_type_;
  alphabet_id_ = other.alphabet_id_;
  read_length_ = other.read_length_;
  number_of_template_segments_minus1_ =
      other.number_of_template_segments_minus1_;
  max_au_data_unit_size_ = other.max_au_data_unit_size_;
  pos_40_bits_flag_ = other.pos_40_bits_flag_;
  qv_depth_ = other.qv_depth_;
  as_depth_ = other.as_depth_;
  class_i_ds_ = std::move(other.class_i_ds_);
  descriptors_ = std::move(other.descriptors_);
  read_group_index_ds_ = std::move(other.read_group_index_ds_);
  multiple_alignments_flag_ = other.multiple_alignments_flag_;
  spliced_reads_flag_ = other.spliced_reads_flag_;
  extended_alignment_flag_ = other.extended_alignment_flag_;
  signature_cfg_ = other.signature_cfg_;
  qv_coding_configs_ = std::move(other.qv_coding_configs_);
  computed_reference_ = other.computed_reference_;
  return *this;
}

// -----------------------------------------------------------------------------

EncodingSet::EncodingSet(const EncodingSet& other)
    : dataset_type_(DataUnit::DatasetType::kAligned),
      alphabet_id_(AlphabetId::kAcgtn),
      read_length_(0),
      number_of_template_segments_minus1_(0),
      max_au_data_unit_size_(0),
      pos_40_bits_flag_(false),
      qv_depth_(0),
      as_depth_(0),
      class_i_ds_(0),
      descriptors_(18),
      read_group_index_ds_(0),
      multiple_alignments_flag_(false),
      spliced_reads_flag_(false),
      extended_alignment_flag_(false),
      qv_coding_configs_(0) {
  *this = other;
}

// -----------------------------------------------------------------------------

EncodingSet::EncodingSet(EncodingSet&& other) noexcept
    : dataset_type_(DataUnit::DatasetType::kAligned),
      alphabet_id_(AlphabetId::kAcgtn),
      read_length_(0),
      number_of_template_segments_minus1_(0),
      max_au_data_unit_size_(0),
      pos_40_bits_flag_(false),
      qv_depth_(0),
      as_depth_(0),
      class_i_ds_(0),
      descriptors_(18),
      read_group_index_ds_(0),
      multiple_alignments_flag_(false),
      spliced_reads_flag_(false),
      extended_alignment_flag_(false),
      qv_coding_configs_(0) {
  *this = std::move(other);
}

// -----------------------------------------------------------------------------

const QualityValues& EncodingSet::GetQvConfig(record::ClassType type) const {
  for (size_t i = 0; i < class_i_ds_.size(); ++i) {
    if (class_i_ds_[i] == type) {
      return *qv_coding_configs_[i];
    }
  }
  UTILS_DIE("No matching qv config " + std::to_string(static_cast<int>(type)) +
            " in parameter set");
}

// -----------------------------------------------------------------------------

void ParameterSet::SetId(const uint8_t id) { parameter_set_id_ = id; }

// -----------------------------------------------------------------------------

void ParameterSet::SetParentId(const uint8_t id) {
  parent_parameter_set_id_ = id;
}

// -----------------------------------------------------------------------------

uint8_t ParameterSet::GetParentId() const { return parent_parameter_set_id_; }

// -----------------------------------------------------------------------------

void EncodingSet::SetQvDepth(const uint8_t qv) { qv_depth_ = qv; }

// -----------------------------------------------------------------------------

bool EncodingSet::operator==(const EncodingSet& ps) const {
  return dataset_type_ == ps.dataset_type_ && alphabet_id_ == ps.alphabet_id_ &&
         read_length_ == ps.read_length_ &&
         number_of_template_segments_minus1_ ==
             ps.number_of_template_segments_minus1_ &&
         max_au_data_unit_size_ == ps.max_au_data_unit_size_ &&
         pos_40_bits_flag_ == ps.pos_40_bits_flag_ &&
         qv_depth_ == ps.qv_depth_ && as_depth_ == ps.as_depth_ &&
         class_i_ds_ == ps.class_i_ds_ && descriptors_ == ps.descriptors_ &&
         read_group_index_ds_ == ps.read_group_index_ds_ &&
         multiple_alignments_flag_ == ps.multiple_alignments_flag_ &&
         spliced_reads_flag_ == ps.spliced_reads_flag_ &&
         extended_alignment_flag_ == ps.extended_alignment_flag_ &&
         signature_cfg_ == ps.signature_cfg_ && QualityValueCmp(ps) &&
         computed_reference_ == ps.computed_reference_;
}

// -----------------------------------------------------------------------------

[[nodiscard]] bool EncodingSet::IsExtendedAlignment() const {
  return extended_alignment_flag_;
}

// -----------------------------------------------------------------------------

void EncodingSet::SetExtendedAlignment(const bool extended_alignment) {
  extended_alignment_flag_ = extended_alignment;
}

// -----------------------------------------------------------------------------

bool EncodingSet::QualityValueCmp(const EncodingSet& ps) const {
  if (ps.qv_coding_configs_.size() != qv_coding_configs_.size()) {
    return false;
  }
  for (size_t i = 0; i < ps.qv_coding_configs_.size(); ++i) {
    if (!qv_coding_configs_[i]->Equals(ps.qv_coding_configs_[i].get())) {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------

ParameterSet::ParameterSet(util::BitReader& bit_reader)
    : DataUnit(DataUnitType::kParameterSet) {
  bit_reader.Read<uint16_t>(10);
  bit_reader.Read<uint32_t>(22);
  parameter_set_id_ = bit_reader.Read<uint8_t>(8);
  parent_parameter_set_id_ = bit_reader.Read<uint8_t>(8);
  set_ = EncodingSet(bit_reader);
}

// -----------------------------------------------------------------------------

ParameterSet::ParameterSet(
    const uint8_t parameter_set_id, const uint8_t parent_parameter_set_id,
    const DatasetType dataset_type, const AlphabetId alphabet_id,
    const uint32_t read_length, const bool paired_end,
    const bool pos_40_bits_flag, const uint8_t qv_depth, const uint8_t as_depth,
    const bool multiple_alignments_flag, const bool spliced_reads_flag,
    const bool extended_alignment_flag)
    : DataUnit(DataUnitType::kParameterSet),
      parameter_set_id_(parameter_set_id),
      parent_parameter_set_id_(parent_parameter_set_id),
      set_(dataset_type, alphabet_id, read_length, paired_end, pos_40_bits_flag,
           qv_depth, as_depth, multiple_alignments_flag, spliced_reads_flag,
           extended_alignment_flag) {}

// -----------------------------------------------------------------------------

ParameterSet::ParameterSet()
    : DataUnit(DataUnitType::kParameterSet),
      parameter_set_id_(0),
      parent_parameter_set_id_(0) {}

// -----------------------------------------------------------------------------

ParameterSet::ParameterSet(uint8_t parameter_set_id,
                           uint8_t parent_parameter_set_id, EncodingSet set)
    : DataUnit(DataUnitType::kParameterSet),
      parameter_set_id_(parameter_set_id),
      parent_parameter_set_id_(parent_parameter_set_id),
      set_(std::move(set)) {}

// -----------------------------------------------------------------------------

bool ParameterSet::operator==(const ParameterSet& parameter_set) const {
  return set_ == parameter_set.set_;
}

// -----------------------------------------------------------------------------

void ParameterSet::PrintDebug(std::ostream& output, uint8_t, uint8_t) const {
  output << "* Parameter set " << static_cast<uint32_t>(GetId());
  if (GetEncodingSet().IsComputedReference()) {
    switch (GetEncodingSet().GetComputedRef().GetAlgorithm()) {
      case ComputedRef::Algorithm::kLocalAssembly:
        output << ", local assembly";
        break;
      case ComputedRef::Algorithm::kGlobalAssembly:
        output << ", global assembly";
        break;
      case ComputedRef::Algorithm::kRefTransform:
        output << ", ref transform";
        break;
      case ComputedRef::Algorithm::kPushIn:
        output << ", push in";
        break;
      case ComputedRef::Algorithm::kReserved:
        output << ", reserved";
        break;
    }
  } else {
    output << ", reference based";
  }
  switch (GetEncodingSet().GetAlphabetId()) {
    case AlphabetId::kAcgtn:
      output << ", alphabet ACTGN";
      break;
    case AlphabetId::kAcgtryswkmbdhvn:
      output << ", alphabet ACGTRYSWKMBDHVN_";
      break;
    default:
      UTILS_DIE("Unknown alphabet ID");
  }
  output << ", " << GetEncodingSet().GetNumberTemplateSegments() << " segments"
         << std::endl;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/paramqv1/qv_coding_config_1.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::quality::paramqv1 {

// -----------------------------------------------------------------------------

QualityValues1::QualityValues1(
    QualityParametersPresetId quality_param_preset_id, const bool reverse_flag)
    : QualityValues(kModeQv1, reverse_flag) {
  quality_parameter_preset_id_ = quality_param_preset_id;
}

// -----------------------------------------------------------------------------

QualityValues1::QualityValues1(const core::GenDesc desc,
                               util::BitReader& reader)
    : QualityValues(kModeQv1, false) {
  (void)desc;
  assert(desc == genie::core::GenDesc::kQv);

  if (reader.Read<bool>(1)) {
    parameter_set_quality_ = ParameterSet(reader);
  } else {
    quality_parameter_preset_id_ = reader.Read<QualityParametersPresetId>(4);
  }
  qv_reverse_flag_ = reader.Read<bool>(1);
}

// -----------------------------------------------------------------------------

void QualityValues1::SetQualityParameters(
    ParameterSet&& parameter_set_qualities) {
  quality_parameter_preset_id_.reset();
  parameter_set_quality_ = std::move(parameter_set_qualities);
}

// -----------------------------------------------------------------------------

void QualityValues1::Write(util::BitWriter& writer) const {
  writer.WriteBits(qv_coding_mode_, 4);
  writer.WriteBits(static_cast<bool>(parameter_set_quality_), 1);
  if (parameter_set_quality_) {
    parameter_set_quality_->write(writer);
  }
  if (quality_parameter_preset_id_) {
    writer.WriteBits(static_cast<uint64_t>(*quality_parameter_preset_id_), 4);
  }
  writer.WriteBits(qv_reverse_flag_, 1);
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::QualityValues> QualityValues1::Clone() const {
  auto ret = std::make_unique<QualityValues1>(
      quality_parameter_preset_id_ ? quality_parameter_preset_id_.value()
                                   : QualityParametersPresetId ::ASCII,
      qv_reverse_flag_);
  if (parameter_set_quality_) {
    auto quality_parameter_set = parameter_set_quality_;
    ret->SetQualityParameters(std::move(quality_parameter_set.value()));
  }
  ret->quality_parameter_preset_id_ = quality_parameter_preset_id_;
  return ret;
}

// -----------------------------------------------------------------------------

std::unique_ptr<QualityValues1::QualityValues> QualityValues1::create(
    core::GenDesc desc, util::BitReader& reader) {
  return std::make_unique<QualityValues1>(desc, reader);
}

// -----------------------------------------------------------------------------

const Codebook& QualityValues1::GetPresetCodebook(
    QualityParametersPresetId id) {
  static const std::vector<Codebook> p_set = []() -> std::vector<Codebook> {
    std::vector<Codebook> ret;
    Codebook set(33, 34);
    for (uint8_t p = 2; p < 94; ++p) {
      set.AddEntry(p + 33);
    }
    ret.emplace_back(std::move(set));

    set = Codebook(33, 41);
    for (uint8_t p = 46; p <= 66; p += 5) {
      set.AddEntry(p);
    }
    set.AddEntry(74);
    ret.emplace_back(std::move(set));

    set = Codebook(64, 72);
    for (uint8_t p = 77; p <= 97; p += 5) {
      set.AddEntry(p);
    }
    set.AddEntry(104);
    ret.emplace_back(std::move(set));
    return ret;
  }();
  return p_set[static_cast<uint8_t>(id)];
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::QualityValues> QualityValues1::GetDefaultSet(
    const core::record::ClassType type) {
  auto ret =
      std::make_unique<QualityValues1>(QualityParametersPresetId::ASCII, false);
  ParameterSet set;

  auto codebook = GetPresetCodebook(QualityParametersPresetId::ASCII);
  set.AddCodeBook(std::move(codebook));

  if (type == core::record::ClassType::kClassI ||
      type == core::record::ClassType::kClassHm) {
    codebook = GetPresetCodebook(QualityParametersPresetId::ASCII);
    set.AddCodeBook(std::move(codebook));
  }
  ret->SetQualityParameters(std::move(set));
  return ret;
}

// -----------------------------------------------------------------------------

size_t QualityValues1::GetNumberCodeBooks() const {
  if (quality_parameter_preset_id_ != std::nullopt) {
    return 1;
  }
  return parameter_set_quality_->GetCodebooks().size();
}

// -----------------------------------------------------------------------------

const Codebook& QualityValues1::GetCodebook(const size_t id) const {
  if (quality_parameter_preset_id_ != std::nullopt) {
    UTILS_DIE_IF(id > 0, "Codebook out of bounds");
    return GetPresetCodebook(quality_parameter_preset_id_.value());
  }
  return parameter_set_quality_->GetCodebooks()[id];
}

// -----------------------------------------------------------------------------

size_t QualityValues1::GetNumSubsequences() const {
  return GetNumberCodeBooks() + 2;
}

// -----------------------------------------------------------------------------

bool QualityValues1::Equals(const QualityValues* qv) const {
  return QualityValues::Equals(qv) &&
         parameter_set_quality_ ==
             dynamic_cast<const QualityValues1*>(qv)->parameter_set_quality_ &&
         quality_parameter_preset_id_ == dynamic_cast<const QualityValues1*>(qv)
                                             ->quality_parameter_preset_id_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::paramqv1

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

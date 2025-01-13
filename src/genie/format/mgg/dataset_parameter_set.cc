/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_parameter_set.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {
// -----------------------------------------------------------------------------

bool DatasetParameterSet::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetParameterSet&>(info);
  return dataset_group_id_ == other.dataset_group_id_ &&
         dataset_id_ == other.dataset_id_ &&
         parameter_set_id_ == other.parameter_set_id_ &&
         parent_parameter_set_id_ == other.parent_parameter_set_id_ &&
         param_update_ == other.param_update_ &&
         param_update_ == other.param_update_ && version_ == other.version_;
}

// -----------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(const uint8_t dataset_group_id,
                                         const uint16_t dataset_id,
                                         const uint8_t parameter_set_id,
                                         const uint8_t parent_parameter_set_id,
                                         core::parameter::EncodingSet ps,
                                         const core::MpegMinorVersion version)
    : dataset_group_id_(dataset_group_id),
      dataset_id_(dataset_id),
      parameter_set_id_(parameter_set_id),
      parent_parameter_set_id_(parent_parameter_set_id),
      params_(std::move(ps)),
      version_(version) {}

// -----------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(util::BitReader& reader,
                                         const core::MpegMinorVersion version,
                                         const bool parameters_update_flag)
    : version_(version) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  dataset_group_id_ = reader.ReadAlignedInt<uint8_t>();
  dataset_id_ = reader.ReadAlignedInt<uint16_t>();
  parameter_set_id_ = reader.ReadAlignedInt<uint8_t>();
  parent_parameter_set_id_ = reader.ReadAlignedInt<uint8_t>();
  if (version_ != core::MpegMinorVersion::kV1900 && parameters_update_flag) {
    param_update_ = dataset_parameter_set::UpdateInfo(reader);
  }
  params_ = core::parameter::EncodingSet(reader);
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

void DatasetParameterSet::BoxWrite(util::BitWriter& writer) const {
  writer.WriteAlignedInt(dataset_group_id_);
  writer.WriteAlignedInt(dataset_id_);
  writer.WriteAlignedInt(parameter_set_id_);
  writer.WriteAlignedInt(parent_parameter_set_id_);
  if (param_update_ != std::nullopt) {
    param_update_->Write(writer);
  }
  params_.Write(writer);
}

// -----------------------------------------------------------------------------

void DatasetParameterSet::AddParameterUpdate(
    dataset_parameter_set::UpdateInfo update) {
  if (version_ != core::MpegMinorVersion::kV1900) {
    param_update_ = update;
  }
}

// -----------------------------------------------------------------------------

const std::string& DatasetParameterSet::GetKey() const {
  static const std::string key = "pars";
  return key;
}

// -----------------------------------------------------------------------------

uint8_t DatasetParameterSet::GetDatasetGroupId() const {
  return dataset_group_id_;
}

// -----------------------------------------------------------------------------

uint16_t DatasetParameterSet::GetDatasetId() const { return dataset_id_; }

// -----------------------------------------------------------------------------

uint8_t DatasetParameterSet::GetParameterSetId() const {
  return parameter_set_id_;
}

// -----------------------------------------------------------------------------

uint8_t DatasetParameterSet::GetParentParameterSetId() const {
  return parent_parameter_set_id_;
}

// -----------------------------------------------------------------------------

bool DatasetParameterSet::HasParameterUpdate() const {
  return param_update_ != std::nullopt;
}

// -----------------------------------------------------------------------------

const dataset_parameter_set::UpdateInfo&
DatasetParameterSet::GetParameterUpdate() const {
  return *param_update_;
}

// -----------------------------------------------------------------------------

const core::parameter::EncodingSet& DatasetParameterSet::GetEncodingSet()
    const {
  return params_;
}

// -----------------------------------------------------------------------------

core::parameter::EncodingSet&& DatasetParameterSet::MoveParameterSet() {
  return std::move(params_);
}

// -----------------------------------------------------------------------------

void DatasetParameterSet::PatchId(const uint8_t group_id,
                                  const uint16_t set_id) {
  dataset_group_id_ = group_id;
  dataset_id_ = set_id;
}

// -----------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(const uint8_t dataset_group_id,
                                         const uint16_t dataset_id,
                                         core::parameter::ParameterSet set,
                                         const core::MpegMinorVersion version)
    : DatasetParameterSet(dataset_group_id, dataset_id, set.GetId(),
                          set.GetParentId(), std::move(set.GetEncodingSet()),
                          version) {}

// -----------------------------------------------------------------------------

core::parameter::ParameterSet DatasetParameterSet::descapsulate() {
  return {parameter_set_id_, parent_parameter_set_id_, std::move(params_)};
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

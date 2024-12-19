/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/descriptor_sub_sequence_cfg.h"

#include <memory>
#include <utility>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

// -----------------------------------------------------------------------------
DescriptorSubSequenceCfg::DescriptorSubSequenceCfg()
    : class_specific_dec_cfg_flag_(false), descriptor_configurations_(0) {
  descriptor_configurations_.push_back(
      std::make_unique<desc_pres::DescriptorPresent>());
}

// -----------------------------------------------------------------------------
DescriptorSubSequenceCfg::DescriptorSubSequenceCfg(
    const DescriptorSubSequenceCfg& cfg)
    : class_specific_dec_cfg_flag_(false) {
  *this = cfg;
}

// -----------------------------------------------------------------------------
DescriptorSubSequenceCfg::DescriptorSubSequenceCfg(
    DescriptorSubSequenceCfg&& cfg) noexcept
    : class_specific_dec_cfg_flag_(false) {
  *this = std::move(cfg);
}

// -----------------------------------------------------------------------------
DescriptorSubSequenceCfg& DescriptorSubSequenceCfg::operator=(
    const DescriptorSubSequenceCfg& cfg) {
  if (this == &cfg) {
    return *this;
  }
  class_specific_dec_cfg_flag_ = cfg.class_specific_dec_cfg_flag_;
  descriptor_configurations_.clear();
  for (const auto& b : cfg.descriptor_configurations_) {
    descriptor_configurations_.emplace_back(b->Clone());
  }
  return *this;
}

// -----------------------------------------------------------------------------
DescriptorSubSequenceCfg& DescriptorSubSequenceCfg::operator=(
    DescriptorSubSequenceCfg&& cfg) noexcept {
  class_specific_dec_cfg_flag_ = cfg.class_specific_dec_cfg_flag_;
  descriptor_configurations_ = std::move(cfg.descriptor_configurations_);
  return *this;
}

// -----------------------------------------------------------------------------
DescriptorSubSequenceCfg::DescriptorSubSequenceCfg(const size_t num_classes,
                                                   const GenDesc desc,
                                                   util::BitReader& reader) {
  class_specific_dec_cfg_flag_ = reader.Read<bool>(1);
  if (class_specific_dec_cfg_flag_ == 0) {
    descriptor_configurations_.emplace_back(Descriptor::Factory(desc, reader));
  } else {
    for (size_t i = 0; i < num_classes; ++i) {
      descriptor_configurations_.emplace_back(
          Descriptor::Factory(desc, reader));
    }
  }
}

// -----------------------------------------------------------------------------
void DescriptorSubSequenceCfg::SetClassSpecific(
    const uint8_t index, std::unique_ptr<Descriptor> conf) {
  if (index > descriptor_configurations_.size()) {
    UTILS_THROW_RUNTIME_EXCEPTION("Config index out of bounds.");
  }
  descriptor_configurations_[index] = std::move(conf);
}

// -----------------------------------------------------------------------------
void DescriptorSubSequenceCfg::Set(std::unique_ptr<Descriptor> conf) {
  if (class_specific_dec_cfg_flag_) {
    UTILS_THROW_RUNTIME_EXCEPTION(
        "Invalid setConfig() for non class-specific descriptor config.");
  }
  descriptor_configurations_[0] = std::move(conf);
}

// -----------------------------------------------------------------------------
const Descriptor& DescriptorSubSequenceCfg::Get() const {
  return *descriptor_configurations_.front();
}

// -----------------------------------------------------------------------------
const Descriptor& DescriptorSubSequenceCfg::GetClassSpecific(
    const uint8_t index) const {
  return *descriptor_configurations_[index];
}

// -----------------------------------------------------------------------------
void DescriptorSubSequenceCfg::EnableClassSpecific(const uint8_t num_classes) {
  if (class_specific_dec_cfg_flag_) {
    return;
  }
  class_specific_dec_cfg_flag_ = true;
  descriptor_configurations_.resize(num_classes);
  for (size_t i = 1; i < descriptor_configurations_.size(); ++i) {
    descriptor_configurations_[i] = descriptor_configurations_[0]->Clone();
  }
}

// -----------------------------------------------------------------------------
bool DescriptorSubSequenceCfg::IsClassSpecific() const {
  return class_specific_dec_cfg_flag_;
}

// -----------------------------------------------------------------------------
void DescriptorSubSequenceCfg::Write(util::BitWriter& writer) const {
  writer.WriteBits(class_specific_dec_cfg_flag_, 1);
  for (auto& i : descriptor_configurations_) {
    i->Write(writer);
  }
}

// -----------------------------------------------------------------------------
bool DescriptorSubSequenceCfg::operator==(
    const DescriptorSubSequenceCfg& cfg) const {
  return class_specific_dec_cfg_flag_ == cfg.class_specific_dec_cfg_flag_ &&
         DescComp(cfg);
}

// -----------------------------------------------------------------------------
bool DescriptorSubSequenceCfg::DescComp(
    const DescriptorSubSequenceCfg& cfg) const {
  if (cfg.descriptor_configurations_.size() !=
      descriptor_configurations_.size()) {
    return false;
  }
  for (size_t i = 0; i < cfg.descriptor_configurations_.size(); ++i) {
    if (!descriptor_configurations_[i]->Equals(
            cfg.descriptor_configurations_[i].get())) {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

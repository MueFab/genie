/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/descriptor.h"

#include <memory>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

// -----------------------------------------------------------------------------

bool Descriptor::Equals(const Descriptor* desc) const {
  return dec_cfg_preset_ == desc->dec_cfg_preset_;
}

// -----------------------------------------------------------------------------

void Descriptor::Write(util::BitWriter& writer) const {
  writer.WriteBits(dec_cfg_preset_, 8);
}
std::unique_ptr<Descriptor> Descriptor::Factory(GenDesc desc,
                                                util::BitReader& reader) {
  const auto preset = reader.Read<uint8_t>();
  UTILS_DIE_IF(preset != 0, "Invalid DecCfgPreset");
  return std::make_unique<desc_pres::DescriptorPresent>(desc, reader);
}

// -----------------------------------------------------------------------------

uint8_t Descriptor::GetPreset() const { return dec_cfg_preset_; }

// -----------------------------------------------------------------------------

Descriptor::Descriptor(const uint8_t dec_cfg_preset)
    : dec_cfg_preset_(dec_cfg_preset) {}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

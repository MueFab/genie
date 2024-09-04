/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/parameter/descriptor.h"
#include "genie/core/parameter/descriptor_present/descriptor_present.h"

#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter {

// ---------------------------------------------------------------------------------------------------------------------

bool Descriptor::equals(const Descriptor *desc) const { return dec_cfg_preset == desc->dec_cfg_preset; }

// ---------------------------------------------------------------------------------------------------------------------

void Descriptor::write(util::BitWriter &writer) const { writer.writeBits(uint8_t(dec_cfg_preset), 8);
}
std::unique_ptr<Descriptor> Descriptor::factory(GenDesc desc, util::BitReader &reader) {
    auto preset = reader.read<uint8_t>();
    UTILS_DIE_IF(preset != 0, "Invalid DecCfgPreset");
    return std::make_unique<desc_pres::DescriptorPresent>(desc, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Descriptor::getPreset() const { return dec_cfg_preset; }

// ---------------------------------------------------------------------------------------------------------------------

Descriptor::Descriptor(uint8_t _dec_cfg_preset) : dec_cfg_preset(_dec_cfg_preset) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

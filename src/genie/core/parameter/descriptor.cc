/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/parameter/descriptor.h"
#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

bool Descriptor::equals(const Descriptor *desc) const { return dec_cfg_preset == desc->dec_cfg_preset; }

// ---------------------------------------------------------------------------------------------------------------------

void Descriptor::write(util::BitWriter &writer) const { writer.write(uint8_t(dec_cfg_preset), 8); }
std::unique_ptr<Descriptor> Descriptor::factory(GenDesc desc, util::BitReader &reader) {
    auto preset = reader.Read<uint8_t>();
    switch (preset) {
        case 0:  // TODO(Fabian): move factory
            return util::make_unique<desc_pres::DescriptorPresent>(desc, reader);
        default:
            UTILS_DIE("Invalid DecCfgPreset");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Descriptor::getPreset() const { return dec_cfg_preset; }

// ---------------------------------------------------------------------------------------------------------------------

Descriptor::Descriptor(uint8_t _dec_cfg_preset) : dec_cfg_preset(_dec_cfg_preset) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

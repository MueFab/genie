/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "descriptor.h"
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>
#include <parameter/descriptor_present/descriptor_present.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

void Descriptor::write(util::BitWriter &writer) const { writer.write(uint8_t(dec_cfg_preset), 8); }
std::unique_ptr<Descriptor> Descriptor::factory(GenDesc desc, util::BitReader &reader) {
    auto preset = Preset(reader.read(8));
    switch (preset) {
        case Preset::PRESENT:
            return util::make_unique<desc_pres::DescriptorPresent>(desc, reader);
        default:
            UTILS_DIE("Invalid DecCfgPreset");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Descriptor::Preset Descriptor::getPreset() const { return dec_cfg_preset; }

// ---------------------------------------------------------------------------------------------------------------------

Descriptor::Descriptor(Preset _dec_cfg_preset) : dec_cfg_preset(_dec_cfg_preset) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
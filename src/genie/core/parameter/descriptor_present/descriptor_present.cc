/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "descriptor_present.h"
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <global-cfg.h>
#include "decoder-regular.h"
#include "decoder-tokentype.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

// ---------------------------------------------------------------------------------------------------------------------

DescriptorPresent::DescriptorPresent() : Descriptor(Preset::PRESENT), decoder_configuration(nullptr) {
    decoder_configuration = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorPresent::DescriptorPresent(GenDesc desc, util::BitReader &reader) : Descriptor(Preset::PRESENT) {
    uint8_t mode = reader.read(8);
    if(desc == GenDesc::MSAR || desc == GenDesc::RNAME) {
        decoder_configuration = GlobalCfg::getSingleton().getIndustrialPark().construct<DecoderTokentype>(mode, reader);
    } else {
        decoder_configuration = GlobalCfg::getSingleton().getIndustrialPark().construct<DecoderRegular>(mode, reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Descriptor> DescriptorPresent::clone() const {
    auto ret = util::make_unique<DescriptorPresent>();
    ret->dec_cfg_preset = dec_cfg_preset;
    ret->decoder_configuration = decoder_configuration->clone();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorPresent::write(util::BitWriter &writer) const {
    Descriptor::write(writer);
    if (dec_cfg_preset != Preset::PRESENT) {
        return;
    }
    decoder_configuration->write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorPresent::setDecoder(std::unique_ptr<Decoder> conf) { decoder_configuration = std::move(conf); }

// ---------------------------------------------------------------------------------------------------------------------

const Decoder *DescriptorPresent::getDecoder() const { return decoder_configuration.get(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
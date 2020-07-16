/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/make-unique.h>

#include <global-cfg.h>
#include "decoder-regular.h"
#include "decoder-tokentype.h"
#include "descriptor_present.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

// ---------------------------------------------------------------------------------------------------------------------

DescriptorPresent::DescriptorPresent() : Descriptor(PRESENT), decoder_configuration(nullptr) {
    decoder_configuration = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorPresent::DescriptorPresent(GenDesc desc, util::BitReader &reader) : Descriptor(PRESENT) {
    auto mode = reader.read<uint8_t>();
    if (desc == GenDesc::MSAR || desc == GenDesc::RNAME) {
        decoder_configuration = GlobalCfg::getSingleton().getIndustrialPark().construct<DecoderTokentype>(mode, reader);
    } else {
        decoder_configuration = GlobalCfg::getSingleton().getIndustrialPark().construct<DecoderRegular>(mode, reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorPresent::equals(const Descriptor *desc) const {
    return Descriptor::equals(desc) &&
           decoder_configuration->equals(dynamic_cast<const DescriptorPresent *>(desc)->decoder_configuration.get());
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
    if (dec_cfg_preset != PRESENT) {
        return;
    }
    decoder_configuration->write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorPresent::setDecoder(std::unique_ptr<Decoder> conf) { decoder_configuration = std::move(conf); }

// ---------------------------------------------------------------------------------------------------------------------

const Decoder &DescriptorPresent::getDecoder() const { return *decoder_configuration; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
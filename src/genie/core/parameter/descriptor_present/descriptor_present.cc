/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include <memory>
#include <utility>
#include "genie/core/global-cfg.h"
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/core/parameter/descriptor_present/decoder-tokentype.h"
#include "genie/util/make_unique.h"

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
    auto mode = reader.Read<uint8_t>();
    if (desc == GenDesc::MSAR || desc == GenDesc::RNAME) {
        decoder_configuration =
            GlobalCfg::getSingleton().getIndustrialPark().construct<DecoderTokentype>(mode, desc, reader);
    } else {
        decoder_configuration =
            GlobalCfg::getSingleton().getIndustrialPark().construct<DecoderRegular>(mode, desc, reader);
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

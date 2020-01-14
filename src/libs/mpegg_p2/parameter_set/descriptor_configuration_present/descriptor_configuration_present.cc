#include "descriptor_configuration_present.h"
#include "mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
DescriptorConfigurationPresent::DescriptorConfigurationPresent()
    : DescriptorConfiguration(DecCfgPreset::PRESENT), decoder_configuration(nullptr) {
    decoder_configuration = util::make_unique<cabac::DecoderConfigurationCabac>(genie::GenDesc::UREADS);
}

DescriptorConfigurationPresent::DescriptorConfigurationPresent(genie::GenDesc desc, util::BitReader &reader)
: DescriptorConfiguration(DecCfgPreset::PRESENT) {
    decoder_configuration = DecoderConfiguration::factory(desc, reader);
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<DescriptorConfiguration> DescriptorConfigurationPresent::clone() const {
    auto ret = util::make_unique<DescriptorConfigurationPresent>();
    ret->dec_cfg_preset = dec_cfg_preset;
    ret->decoder_configuration = decoder_configuration->clone();
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

void DescriptorConfigurationPresent::write(util::BitWriter &writer) const {
    DescriptorConfiguration::write(writer);
    if (dec_cfg_preset != DecCfgPreset::PRESENT) {
        return;
    }
    decoder_configuration->write(writer);
}

// -----------------------------------------------------------------------------------------------------------------

void DescriptorConfigurationPresent::set_decoder_configuration(std::unique_ptr<DecoderConfiguration> conf) {
    decoder_configuration = std::move(conf);
}

const DecoderConfiguration *DescriptorConfigurationPresent::getDecoderConfiguration() const { return decoder_configuration.get(); }

}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie
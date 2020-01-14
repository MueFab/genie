#include "descriptor_configuration_present.h"
#include "cabac/decoder_configuration_cabac.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

namespace format {
namespace mpegg_p2 {
namespace desc_conf_pres {
DescriptorConfigurationPresent::DescriptorConfigurationPresent()
    : DescriptorConfiguration(DecCfgPreset::PRESENT), decoder_configuration(nullptr) {
    decoder_configuration = util::make_unique<cabac::DecoderConfigurationCabac>(coding::GenDesc::UREADS);
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
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

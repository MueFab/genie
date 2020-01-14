#include "descriptor_configuration.h"
#include <mpegg_p2/parameter_set/descriptor_configuration_present/descriptor_configuration_present.h>
#include <util/bitreader.h>
#include <util/make_unique.h>
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
void DescriptorConfiguration::write(util::BitWriter &writer) const { writer.write(uint8_t(dec_cfg_preset), 8); }
std::unique_ptr<DescriptorConfiguration> DescriptorConfiguration::factory(genie::GenDesc desc,
                                                                          util::BitReader &reader) {
    auto preset = DecCfgPreset(reader.read(8));
    switch (preset) {
        case DecCfgPreset::PRESENT:
            return util::make_unique<desc_conf_pres::DescriptorConfigurationPresent>(desc, reader);
        default:
            UTILS_DIE("Invalid DecCfgPreset");
    }
}

DescriptorConfiguration::DecCfgPreset DescriptorConfiguration::getType() const { return dec_cfg_preset; }

// -----------------------------------------------------------------------------------------------------------------

DescriptorConfiguration::DescriptorConfiguration(DecCfgPreset _dec_cfg_preset) : dec_cfg_preset(_dec_cfg_preset) {}
}  // namespace mpegg_p2
}  // namespace genie
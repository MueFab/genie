#include "descriptor_configuration_present.h"
#include "ureads-encoder/format/part2/make_unique.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac_regular.h"

namespace format {
    namespace desc_conf_pres {
        DescriptorConfigurationPresent::DescriptorConfigurationPresent() : DescriptorConfiguration(
                DecCfgPreset::PRESENT),
                                                                           decoder_configuration(nullptr) {
            decoder_configuration = make_unique<cabac::DecoderConfigurationCabacRegular>();

        }

        // -----------------------------------------------------------------------------------------------------------------

        std::unique_ptr<DescriptorConfiguration> DescriptorConfigurationPresent::clone() const {
            auto ret = make_unique<DescriptorConfigurationPresent>();
            ret->dec_cfg_preset = dec_cfg_preset;
            ret->decoder_configuration = decoder_configuration->clone();
            return ret;
        }

        // -----------------------------------------------------------------------------------------------------------------

        void DescriptorConfigurationPresent::write(BitWriter *writer) const {
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

        // -----------------------------------------------------------------------------------------------------------------

        void DescriptorConfigurationPresent::_deactivate() {
            /*  decoder_configuration_tokentype.resize(1);
              decoder_configuration_tokentype[0] = conf;
              decoder_configuration.clear();*/
            dec_cfg_preset = DecCfgPreset(255); // TODO: implement token type, don't rely on reserved values
        }
    }
}
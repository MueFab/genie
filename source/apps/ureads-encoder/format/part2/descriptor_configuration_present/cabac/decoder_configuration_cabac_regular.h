#ifndef GENIE_DECODER_CONFIGURATION_CABAC_REGULAR_H
#define GENIE_DECODER_CONFIGURATION_CABAC_REGULAR_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

#include "ureads-encoder/format/part2/descriptor_configuration_present/decoder_configuration.h"
#include "decoder_configuration_cabac.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        namespace cabac {
            /**
            * ISO 23092-2 Section 8.3.1 table line 3 to 13
            */
            class DecoderConfigurationCabacRegular : public DecoderConfigurationCabac {
            private:
                uint8_t num_descriptor_subsequence_cfgs_minus1 : 8; //!< Line 3
                std::vector<std::unique_ptr<DescriptorSubsequenceCfg>> descriptor_subsequence_cfgs; //!< Line 4 to 13
            public:
                DecoderConfigurationCabacRegular();

                void addSubsequenceCfg(std::unique_ptr<DescriptorSubsequenceCfg> cfg);

                std::unique_ptr<DecoderConfiguration> clone() const override;

                void write(BitWriter *writer) const override;
            };
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_DECODER_CONFIGURATION_CABAC_REGULAR_H

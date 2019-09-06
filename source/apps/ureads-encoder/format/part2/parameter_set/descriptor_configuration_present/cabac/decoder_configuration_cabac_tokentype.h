#ifndef GENIE_DECODER_CONFIGURATION_CABAC_TOKENTYPE_H
#define GENIE_DECODER_CONFIGURATION_CABAC_TOKENTYPE_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>

// -----------------------------------------------------------------------------------------------------------------

#include "descriptor_subsequence_cfg.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        namespace cabac {
            /**
            * ISO 23092-2 Section 8.3.5 table 101
            */
            class DecoderConfigurationCabacTokentype : public DecoderConfigurationCabac{
            private:
                uint8_t rle_guard_tokentype : 8; //!< line 4
                std::unique_ptr<DescriptorSubsequenceCfg> decoder_configuration_tokentype_cabac_0; //!< line 6
                std::unique_ptr<DescriptorSubsequenceCfg> decoder_configuration_tokentype_cabac_1; //!< line 8

            public:
                DecoderConfigurationCabacTokentype();

                void addSubsequenceCfg(std::unique_ptr<DescriptorSubsequenceCfg> cfg);

                std::unique_ptr<DecoderConfiguration> clone() const override;

                void write(BitWriter *writer) const override;

            };
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_DECODER_CONFIGURATION_CABAC_TOKENTYPE_H

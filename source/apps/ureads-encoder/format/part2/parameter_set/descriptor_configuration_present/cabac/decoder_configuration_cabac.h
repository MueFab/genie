#ifndef GENIE_DECODER_CONFIGURATION_CABAC_H
#define GENIE_DECODER_CONFIGURATION_CABAC_H

// -----------------------------------------------------------------------------------------------------------------

#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/decoder_configuration.h"
#include "ureads-encoder/format/part2/clutter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        namespace cabac {
            /**
            * Base for ISO 23092-2 Section 8.3.1 and ISO 23092-2 Section 8.3.5
            */
            class DecoderConfigurationCabac : public DecoderConfiguration {
            protected:
                GenomicDescriptor desc;
                uint8_t rle_guard_tokentype : 8; //!< line 4
                std::vector<std::unique_ptr<DescriptorSubsequenceCfg>> descriptor_subsequence_cfgs; //!< Line 4 to 13

            public:
                explicit DecoderConfigurationCabac(GenomicDescriptor _desc);
                void setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformSubseqParameters> cfg);
                DescriptorSubsequenceCfg* getSubsequenceCfg(uint8_t index) const;
                std::unique_ptr<DecoderConfiguration> clone() const override;

                void write(BitWriter *writer) const override;
            };
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_DECODER_CONFIGURATION_CABAC_H

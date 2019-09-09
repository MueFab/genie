#ifndef GENIE_DECODER_CONFIGURATION_CABAC_H
#define GENIE_DECODER_CONFIGURATION_CABAC_H

// -----------------------------------------------------------------------------------------------------------------

#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/decoder_configuration.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        namespace cabac {
            /**
            * Base for ISO 23092-2 Section 8.3.1 and ISO 23092-2 Section 8.3.5
            */
            class DecoderConfigurationCabac : public DecoderConfiguration {
            protected:

            public:
                virtual void addSubsequenceCfg(std::unique_ptr<DescriptorSubsequenceCfg> cfg) = 0;
                DecoderConfigurationCabac();
            };
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_DECODER_CONFIGURATION_CABAC_H

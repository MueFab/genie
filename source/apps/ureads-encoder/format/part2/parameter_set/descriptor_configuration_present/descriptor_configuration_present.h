#ifndef GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H
#define GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H

// -----------------------------------------------------------------------------------------------------------------

#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration.h"
#include "decoder_configuration.h"

#include <memory>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        /**
        * ISO 23092-2 Section 3.3.2.1 table 8, lines 4 to 9
        */
        class DescriptorConfigurationPresent : public DescriptorConfiguration {
        private:
            std::unique_ptr<DecoderConfiguration> decoder_configuration; //!< Line 5 to 9 (fused)
        public:
            explicit DescriptorConfigurationPresent();

            std::unique_ptr<DescriptorConfiguration> clone() const override;

            void write(BitWriter *writer) const override;

            void set_decoder_configuration(std::unique_ptr<DecoderConfiguration> conf);
        };
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H

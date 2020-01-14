#ifndef GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H
#define GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H

// -----------------------------------------------------------------------------------------------------------------

#include "../descriptor_configuration.h"
#include "decoder_configuration.h"
#include "util/bitreader.h"
#include "util/bitwriter.h"

#include <util/make_unique.h>
#include <memory>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
namespace desc_conf_pres {
/**
 * ISO 23092-2 Section 3.3.2.1 table 8, lines 4 to 9
 */
class DescriptorConfigurationPresent : public DescriptorConfiguration {
   private:
    std::unique_ptr<DecoderConfiguration> decoder_configuration;  //!< Line 5 to 9 (fused)
   public:
    explicit DescriptorConfigurationPresent();

    explicit DescriptorConfigurationPresent(GenDesc desc, util::BitReader &reader)
        : DescriptorConfiguration(DecCfgPreset::PRESENT) {
        decoder_configuration = DecoderConfiguration::factory(desc, reader);
    }

    std::unique_ptr<DescriptorConfiguration> clone() const override;

    void write(util::BitWriter &writer) const override;

    void set_decoder_configuration(std::unique_ptr<DecoderConfiguration> conf);

    const DecoderConfiguration *getDecoderConfiguration() const { return decoder_configuration.get(); }
};
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H

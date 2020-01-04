#ifndef GENIE_DESCRIPTOR_CONFIGURATION_H
#define GENIE_DESCRIPTOR_CONFIGURATION_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include "util/bitwriter.h"
#include "util/bitreader.h"
#include <coding/constants.h>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace mpegg_p2 {
/**
 * ISO 23092-2 Section 3.3.2.1 table 8
 */
        class DescriptorConfiguration {
        public:
            enum class DecCfgPreset : uint8_t {
                PRESENT = 0  //!< See text in section 3.3.2.1
            };

            explicit DescriptorConfiguration(DecCfgPreset _dec_cfg_preset);

            virtual std::unique_ptr<DescriptorConfiguration> clone() const = 0;

            virtual void write(util::BitWriter &writer) const;

            static std::unique_ptr<DescriptorConfiguration> factory(GenDesc desc, util::BitReader &reader);

            virtual ~DescriptorConfiguration() = default;

            DecCfgPreset getType() const {
                return dec_cfg_preset;
            }

        protected:
            DecCfgPreset dec_cfg_preset : 8;  //!< Line 2
        };
    }
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_CONFIGURATION_H

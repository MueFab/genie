#ifndef GENIE_DESCRIPTOR_CONFIGURATION_CONTAINER_H
#define GENIE_DESCRIPTOR_CONFIGURATION_CONTAINER_H

// -----------------------------------------------------------------------------------------------------------------

#include "descriptor_configuration.h"
#include "util/bitwriter.h"

#include <util/make_unique.h>
#include <memory>
#include <vector>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {

/**
 * ISO 23092-2 Section 3.3.2 table 7 lines 15 to 21
 */
class DescriptorConfigurationContainer {
   public:
    DescriptorConfigurationContainer();

    DescriptorConfigurationContainer(size_t num_classes, GenDesc desc, util::BitReader& reader) {
        class_specific_dec_cfg_flag = reader.read(1);
        if (class_specific_dec_cfg_flag == 0) {
            descriptor_configurations.emplace_back(DescriptorConfiguration::factory(desc, reader));
        } else {
            for (size_t i = 0; i < num_classes; ++i) {
                descriptor_configurations.emplace_back(DescriptorConfiguration::factory(desc, reader));
            }
        }
    }

    virtual ~DescriptorConfigurationContainer() = default;

    void setConfig(uint8_t index, std::unique_ptr<DescriptorConfiguration> conf);  //!< For class specific config
    void setConfig(std::unique_ptr<DescriptorConfiguration> conf);                 //!< For non-class-specific config
    const DescriptorConfiguration* getConfig() const { return descriptor_configurations.front().get(); }
    const DescriptorConfiguration* getConfig(uint8_t index) const { return descriptor_configurations[index].get(); }
    void enableClassSpecificConfigs(uint8_t numClasses);  //!< Unlocks class specific config
    bool isClassSpecific() const;

    virtual void write(util::BitWriter& writer) const;

    std::unique_ptr<DescriptorConfigurationContainer> clone() {
        auto ret = util::make_unique<DescriptorConfigurationContainer>();
        ret->class_specific_dec_cfg_flag = class_specific_dec_cfg_flag;
        for (const auto& d : descriptor_configurations) {
            ret->descriptor_configurations.emplace_back(d->clone());
        }
        return ret;
    }

   private:
    bool class_specific_dec_cfg_flag : 1;  //!< Line 15
    std::vector<std::unique_ptr<DescriptorConfiguration>> descriptor_configurations;
};
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_CONFIGURATION_CONTAINER_H

#ifndef GENIE_DESCRIPTOR_CONFIGURATION_CONTAINER_H
#define GENIE_DESCRIPTOR_CONFIGURATION_CONTAINER_H

// -----------------------------------------------------------------------------------------------------------------

#include "descriptor_configuration.h"
#include "util/bitwriter.h"

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

    virtual ~DescriptorConfigurationContainer() = default;

    void setConfig(uint8_t index, std::unique_ptr<DescriptorConfiguration> conf);  //!< For class specific config
    void setConfig(std::unique_ptr<DescriptorConfiguration> conf);                 //!< For non-class-specific config
    const DescriptorConfiguration* getConfig() const {
        return descriptor_configurations.front().get();
    }
    const DescriptorConfiguration* getConfig(uint8_t index) const {
        return descriptor_configurations[index].get();
    }
    void enableClassSpecificConfigs(uint8_t numClasses);                           //!< Unlocks class specific config
    bool isClassSpecific() const;

    virtual void write(util::BitWriter *writer) const;

   private:
    bool class_specific_dec_cfg_flag : 1;  //!< Line 15
    std::vector<std::unique_ptr<DescriptorConfiguration>> descriptor_configurations;
};
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_CONFIGURATION_CONTAINER_H

#ifndef GENIE_DESCRIPTOR_CONFIGURATION_H
#define GENIE_DESCRIPTOR_CONFIGURATION_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
class BitWriter;

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

    virtual void write(BitWriter *writer) const;

    virtual ~DescriptorConfiguration() = default;

   protected:
    DecCfgPreset dec_cfg_preset : 8;  //!< Line 2
};
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_CONFIGURATION_H

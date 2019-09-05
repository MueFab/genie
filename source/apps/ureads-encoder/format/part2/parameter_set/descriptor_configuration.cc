#include "descriptor_configuration.h"
#include "ureads-encoder/format/part2/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    void DescriptorConfiguration::write(BitWriter *writer) const {
        writer->write(uint8_t(dec_cfg_preset), 8);
    }

    // -----------------------------------------------------------------------------------------------------------------

    DescriptorConfiguration::DescriptorConfiguration(DecCfgPreset _dec_cfg_preset) : dec_cfg_preset(_dec_cfg_preset) {

    }
}
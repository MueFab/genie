#include "decoder_configuration.h"
#include <util/bitreader.h>
#include <util/make_unique.h>
#include "mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
void DecoderConfiguration::write(util::BitWriter& writer) const { writer.write(uint8_t(encoding_mode_ID), 8); }

DecoderConfiguration::EncodingModeId DecoderConfiguration::getEncodingMode() const { return encoding_mode_ID; }

std::unique_ptr<DecoderConfiguration> DecoderConfiguration::factory(genie::GenDesc desc, util::BitReader& reader) {
    auto mode = EncodingModeId(reader.read(8));
    switch (mode) {
        case EncodingModeId::CABAC:
            return util::make_unique<cabac::DecoderConfigurationCabac>(desc, reader);
        default:
            UTILS_DIE("Invalid encoding mode id");
    }
}

// -----------------------------------------------------------------------------------------------------------------

DecoderConfiguration::DecoderConfiguration(EncodingModeId _encoding_mode_id) : encoding_mode_ID(_encoding_mode_id) {}
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie
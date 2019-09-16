#include "decoder_configuration.h"
#include "ureads-encoder/format/part2/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        void DecoderConfiguration::write(BitWriter *writer) const {
            writer->write(uint8_t(encoding_mode_ID), 8);
        }

        // -----------------------------------------------------------------------------------------------------------------

        DecoderConfiguration::DecoderConfiguration(EncodingModeId _encoding_mode_id) : encoding_mode_ID(
                _encoding_mode_id) {

        }
    }
}
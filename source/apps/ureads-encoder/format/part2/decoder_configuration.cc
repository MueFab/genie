#include "decoder_configuration.h"
#include "bitwriter.h"

void Decoder_configuration::write(BitWriter *writer) const {
    writer->write(uint8_t (encoding_mode_ID), 8);
}

Decoder_configuration::Decoder_configuration(Encoding_mode_ID _encoding_mode_id) : encoding_mode_ID(_encoding_mode_id) {

}
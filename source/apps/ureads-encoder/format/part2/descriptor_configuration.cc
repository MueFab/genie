#include "descriptor_configuration.h"
#include "bitwriter.h"

void Descriptor_configuration::write(BitWriter *writer) const {
    writer->write(uint8_t(dec_cfg_preset), 8);
}

Descriptor_configuration::Descriptor_configuration(Dec_cfg_preset _dec_cfg_preset) : dec_cfg_preset(_dec_cfg_preset) {

}
#include "descriptor_configuration_present.h"
#include "make_unique.h"
#include "decoder_configuration_cabac_regular.h"

Descriptor_configuration_present::Descriptor_configuration_present() : Descriptor_configuration(
        Dec_cfg_preset::PRESENT),
                                                                       decoder_configuration(nullptr) {
    decoder_configuration = make_unique<Decoder_configuration_cabac_regular>();

}

std::unique_ptr<Descriptor_configuration> Descriptor_configuration_present::clone() const {
    auto ret = make_unique<Descriptor_configuration_present>();
    ret->dec_cfg_preset = dec_cfg_preset;
    ret->decoder_configuration = decoder_configuration->clone();
    return ret;
}

void Descriptor_configuration_present::write(BitWriter *writer) const {
    Descriptor_configuration::write(writer);
    if (dec_cfg_preset != Dec_cfg_preset::PRESENT) {
        return;
    }
    decoder_configuration->write(writer);
}

void Descriptor_configuration_present::set_decoder_configuration(std::unique_ptr<Decoder_configuration> conf) {
    decoder_configuration = std::move(conf);
}

void Descriptor_configuration_present::_deactivate() {
    /*  decoder_configuration_tokentype.resize(1);
      decoder_configuration_tokentype[0] = conf;
      decoder_configuration.clear();*/
    dec_cfg_preset = Dec_cfg_preset(255); // TODO: implement token type, don't rely on reserved values
}
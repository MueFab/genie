#include "decoder_configuration_cabac_regular.h"
#include "make_unique.h"
#include "bitwriter.h"

Decoder_configuration_cabac_regular::Decoder_configuration_cabac_regular() : num_descriptor_subsequence_cfgs_minus1(-1),
                                                                             descriptor_subsequence_cfgs(0) {

}

std::unique_ptr<Decoder_configuration> Decoder_configuration_cabac_regular::clone() const {
    auto ret = make_unique<Decoder_configuration_cabac_regular>();
    ret->encoding_mode_ID = encoding_mode_ID;
    for (const auto &i : descriptor_subsequence_cfgs) {
        ret->addSubsequenceCfg(i->clone());
    }
    return ret;
}

void Decoder_configuration_cabac_regular::addSubsequenceCfg(std::unique_ptr<Descriptor_subsequence_cfg> cfg) {
    num_descriptor_subsequence_cfgs_minus1 += 1;
    descriptor_subsequence_cfgs.push_back(std::move(cfg));
}

void Decoder_configuration_cabac_regular::write(BitWriter *writer) const {
    Decoder_configuration_cabac::write(writer);
    writer->write(num_descriptor_subsequence_cfgs_minus1, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i->write(writer);
    }
}
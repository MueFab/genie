#include "decoder_configuration_cabac.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
DecoderConfigurationCabac::DecoderConfigurationCabac(GenDesc _desc)
    : DecoderConfiguration(EncodingModeId::CABAC), desc(_desc), rle_guard_tokentype(0), descriptor_subsequence_cfgs() {
    for (size_t i = 0; i < getDescriptors()[uint8_t(desc)].subseqs.size(); ++i) {
        descriptor_subsequence_cfgs.push_back(util::make_unique<DescriptorSubsequenceCfg>(
            util::make_unique<TransformSubseqParameters>(), i,
            desc == GenDesc::MSAR || desc == GenDesc::RNAME));
    }
}

void DecoderConfigurationCabac::setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformSubseqParameters> cfg) {
    descriptor_subsequence_cfgs[uint8_t (index)] = util::make_unique<DescriptorSubsequenceCfg>(
        std::move(cfg), uint8_t (index),
        desc == GenDesc::MSAR || desc == GenDesc::RNAME);
}

std::unique_ptr<DecoderConfiguration> DecoderConfigurationCabac::clone() const {
    auto ret = util::make_unique<DecoderConfigurationCabac>(desc);
    ret->encoding_mode_ID = encoding_mode_ID;
    ret->rle_guard_tokentype = rle_guard_tokentype;
    for (size_t i = 0; i < descriptor_subsequence_cfgs.size(); ++i) {
        ret->descriptor_subsequence_cfgs[i] = descriptor_subsequence_cfgs[i]->clone();
    }
    return ret;
}

void DecoderConfigurationCabac::write(util::BitWriter *writer) const {
    DecoderConfiguration::write(writer);
    if (desc == GenDesc::RNAME || desc == GenDesc::MSAR) {
        writer->write(rle_guard_tokentype, 8);
    } else {
        writer->write(descriptor_subsequence_cfgs.size() - 1, 8);
    }
    for (auto &i : descriptor_subsequence_cfgs) {
        i->write(writer);
    }
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

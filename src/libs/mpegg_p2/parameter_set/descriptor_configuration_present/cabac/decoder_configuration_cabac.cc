#include "decoder_configuration_cabac.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
DecoderConfigurationCabac::DecoderConfigurationCabac(genie::GenDesc _desc)
    : DecoderConfiguration(EncodingModeId::CABAC), desc(_desc), rle_guard_tokentype(0), descriptor_subsequence_cfgs() {
    for (size_t i = 0; i < genie::getDescriptors()[uint8_t(desc)].subseqs.size(); ++i) {
        descriptor_subsequence_cfgs.push_back(
            util::make_unique<DescriptorSubsequenceCfg>(util::make_unique<TransformSubseqParameters>(), i,
                                                        desc == genie::GenDesc::MSAR || desc == genie::GenDesc::RNAME));
    }
}

DecoderConfigurationCabac::DecoderConfigurationCabac(genie::GenDesc _desc, util::BitReader &reader)
: DecoderConfiguration(DecoderConfiguration::EncodingModeId::CABAC) {
    desc = _desc;
    uint8_t num_descriptor_subsequence_cfgs = getDescriptor(desc).subseqs.size();
    if (_desc != genie::GenDesc::MSAR && _desc != genie::GenDesc::RNAME) {
        num_descriptor_subsequence_cfgs = reader.read(8) + 1;
    } else {
        rle_guard_tokentype = reader.read(8);
    }
    for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
        descriptor_subsequence_cfgs.emplace_back(util::make_unique<DescriptorSubsequenceCfg>(
            _desc == genie::GenDesc::MSAR || _desc == genie::GenDesc::RNAME, reader));
    }
}

void DecoderConfigurationCabac::setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformSubseqParameters> cfg) {
    descriptor_subsequence_cfgs[uint8_t(index)] = util::make_unique<DescriptorSubsequenceCfg>(
        std::move(cfg), uint8_t(index), desc == genie::GenDesc::MSAR || desc == genie::GenDesc::RNAME);
}

DescriptorSubsequenceCfg *DecoderConfigurationCabac::getSubsequenceCfg(uint8_t index) const {
    return descriptor_subsequence_cfgs[uint8_t(index)].get();
}

DecoderConfigurationCabac::DecoderConfigurationCabac(const DecoderConfigurationCabac &c)
    : DecoderConfiguration(EncodingModeId::CABAC), desc(c.desc), rle_guard_tokentype(c.rle_guard_tokentype) {
    if (this == &c) {
        return;
    }
    for (const auto &a : c.descriptor_subsequence_cfgs) {
        descriptor_subsequence_cfgs.emplace_back(a->clone());
    }
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

void DecoderConfigurationCabac::write(util::BitWriter &writer) const {
    DecoderConfiguration::write(writer);
    if (desc == genie::GenDesc::RNAME || desc == genie::GenDesc::MSAR) {
        writer.write(rle_guard_tokentype, 8);
    } else {
        writer.write(descriptor_subsequence_cfgs.size() - 1, 8);
    }
    for (auto &i : descriptor_subsequence_cfgs) {
        i->write(writer);
    }
}

uint8_t DecoderConfigurationCabac::getRleGuardTokentype() const { return rle_guard_tokentype; }

}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie
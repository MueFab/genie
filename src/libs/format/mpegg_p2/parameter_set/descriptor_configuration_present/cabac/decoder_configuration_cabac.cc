#include "decoder_configuration_cabac.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
DecoderConfigurationCabac::DecoderConfigurationCabac(GenomicDescriptor _desc)
    : DecoderConfiguration(EncodingModeId::CABAC), desc(_desc), rle_guard_tokentype(0), descriptor_subsequence_cfgs(0) {
    for (size_t i = 0; i < getDescriptorProperties()[uint8_t(desc)].number_subsequences; ++i) {
        descriptor_subsequence_cfgs.push_back(
            make_unique<DescriptorSubsequenceCfg>(make_unique<TransformSubseqParameters>(), i,
                                                  desc == GenomicDescriptor::msar || desc == GenomicDescriptor::rname));
    }
}

void DecoderConfigurationCabac::setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformSubseqParameters> cfg) {
    descriptor_subsequence_cfgs[index] = make_unique<DescriptorSubsequenceCfg>(
        std::move(cfg), index, desc == GenomicDescriptor::msar || desc == GenomicDescriptor::rname);
}

DescriptorSubsequenceCfg *DecoderConfigurationCabac::getSubsequenceCfg(uint8_t index) const {
    return descriptor_subsequence_cfgs[index].get();
}

std::unique_ptr<DecoderConfiguration> DecoderConfigurationCabac::clone() const {
    auto ret = make_unique<DecoderConfigurationCabac>(desc);
    ret->encoding_mode_ID = encoding_mode_ID;
    ret->rle_guard_tokentype = rle_guard_tokentype;
    for (size_t i = 0; i < descriptor_subsequence_cfgs.size(); ++i) {
        ret->descriptor_subsequence_cfgs[i] = descriptor_subsequence_cfgs[i]->clone();
    }
    return ret;
}

void DecoderConfigurationCabac::write(util::BitWriter *writer) const {
    DecoderConfiguration::write(writer);
    if (desc == GenomicDescriptor::rname || desc == GenomicDescriptor::msar) {
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
}  // namespace format

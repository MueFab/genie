#ifndef GENIE_NULLENCODER_H
#define GENIE_NULLENCODER_H

#include <genie/core/qv-encoder.h>
#include <genie/quality/paramqv1/qv_coding_config_1.h>

namespace genie {
namespace quality {
namespace paramqv1 {

class NullEncoder : public core::QVEncoder {
    core::QVEncoder::QVCoded encode(const core::record::Chunk &rec) override {
        auto param = util::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
        paramqv1::ParameterSet set;

        auto codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
        set.addCodeBook(std::move(codebook));

        core::AccessUnitRaw::Descriptor desc(core::GenDesc::QV);
        desc.add(core::AccessUnitRaw::Subsequence(1, core::GenSub::QV_PRESENT));
        desc.add(core::AccessUnitRaw::Subsequence(1, core::GenSub::QV_CODEBOOK));
        desc.add(core::AccessUnitRaw::Subsequence(1, core::GenSub::QV_STEPS_0));
        param->setQvps(std::move(set));
        return std::make_pair(std::move(param), std::move(desc));
    }
};

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

#endif  // GENIE_ENCODER_H

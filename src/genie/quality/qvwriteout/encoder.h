#ifndef GENIE_QVOUTENCODER_H
#define GENIE_QVOUTENCODER_H

#include <genie/core/qv-encoder.h>
#include <genie/quality/paramqv1/qv_coding_config_1.h>

namespace genie {
namespace quality {
namespace qvwriteout {

class Encoder : public core::QVEncoder {
    core::QVEncoder::QVCoded encode(const core::record::Chunk &rec) override {
        auto param = util::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
        paramqv1::ParameterSet set;

        auto codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
        set.addCodeBook(std::move(codebook));

        core::AccessUnitRaw::Descriptor desc(core::GenDesc::QV);
        desc.add(core::AccessUnitRaw::Subsequence(4, core::GenSub::QV_PRESENT));
        desc.add(core::AccessUnitRaw::Subsequence(4, core::GenSub::QV_CODEBOOK));
        desc.add(core::AccessUnitRaw::Subsequence(4, core::GenSub::QV_STEPS_0));
        if (rec.front().getClassID() == core::record::ClassType::CLASS_I ||
            rec.front().getClassID() == core::record::ClassType::CLASS_HM) {
            //    desc.add(core::AccessUnitRaw::Subsequence(4, core::GenSub::QV_STEPS_1));

            //  codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
            //  set.addCodeBook(std::move(codebook));
        }
        param->setQvps(std::move(set));
        for (const auto &r : rec) {
            for (const auto &s : r.getSegments()) {
                for (const auto &q : s.getQualities()) {
                    for (const auto &c : q) {
                        UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
                        desc.get(2).push(c - 33);
                    }
                }
            }
        }
        return std::make_pair(std::move(param), std::move(desc));
    }
};

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

#endif  // GENIE_ENCODER_H

#ifndef GENIE_ENCODER_H
#define GENIE_ENCODER_H

#include <core/qv-encoder.h>
#include <quality/paramqv1/qv_coding_config_1.h>

namespace genie {
namespace quality {
namespace qvwriteout {

class Encoder : public core::QVEncoder {
    core::QVEncoder::QVCoded encode(const core::record::Chunk& rec) override {
        core::QVEncoder::QVCoded ret = std::make_pair(util::make_unique<paramqv1::QualityValues1>(), core::AccessUnitRaw::Descriptor(core::GenDesc::QV));
        return ret;
    }
};

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

#endif  // GENIE_ENCODER_H

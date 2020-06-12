#ifndef GENIE_QVOUTENCODER_H
#define GENIE_QVOUTENCODER_H

#include <genie/core/cigar-tokenizer.h>
#include <genie/core/qv-encoder.h>
#include <genie/quality/paramqv1/qv_coding_config_1.h>
#include <genie/util/stringview.h>

namespace genie {
namespace quality {
namespace qvwriteout {

class Encoder : public core::QVEncoder {
   private:
    static void setUpParameters(const core::record::Chunk& rec, paramqv1::QualityValues1& param,
                                core::AccessUnit::Descriptor& desc);

    static void encodeAlignedSegment(const core::record::Segment& s, const std::string& ecigar,
                                     core::AccessUnit::Descriptor& desc);

    static void encodeUnalignedSegment(const core::record::Segment& s, core::AccessUnit::Descriptor& desc);

   public:
    core::QVEncoder::QVCoded process(const core::record::Chunk& rec) override;
};

class NoneEncoder : public core::QVEncoder {
   public:
    using QVCoded = std::pair<std::unique_ptr<core::parameter::QualityValues>, core::AccessUnit::Descriptor>;
    QVCoded process(const core::record::Chunk&) override {
        auto param = util::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
        core::AccessUnit::Descriptor desc(core::GenDesc::QV);

        return std::make_pair(std::move(param), std::move(desc));
    }
};

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

#endif  // GENIE_ENCODER_H

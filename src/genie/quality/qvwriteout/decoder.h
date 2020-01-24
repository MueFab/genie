#ifndef GENIE_DECODER_H
#define GENIE_DECODER_H

#include <genie/core/qv-decoder.h>
#include <genie/quality/paramqv1/qv_coding_config_1.h>

namespace genie {
namespace quality {
namespace qvwriteout {

class Decoder : public core::QVDecoder {
   public:
    std::string decode(const core::parameter::QualityValues& param, size_t length,
                       core::AccessUnitRaw::Descriptor& desc) override {
        std::string qv;
        const auto& param_casted = dynamic_cast<const quality::paramqv1::QualityValues1&>(param);
        if (!desc.get(0).end() && !desc.get(0).pull()) {
            return qv;
        }
        qv.resize(length);
        for (auto& q : qv) {
            uint8_t codebook = desc.get(1).end() ? 0 : desc.get(1).pull();
            uint8_t index = desc.get(codebook + 2).pull();
            q = param_casted.getCodebook(codebook).getEntries()[index];
        }
        return qv;
    }

    static std::unique_ptr<core::QVDecoder> create(util::BitReader& reader) {
        (void)reader;
        return util::make_unique<Decoder>();
    }
};

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

#endif  // GENIE_DECODER_H

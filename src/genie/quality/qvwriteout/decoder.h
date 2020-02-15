#ifndef GENIE_DECODER_H
#define GENIE_DECODER_H

#include <genie/core/cigar-tokenizer.h>
#include <genie/core/qv-decoder.h>
#include <genie/quality/paramqv1/qv_coding_config_1.h>

namespace genie {
namespace quality {
namespace qvwriteout {

class Decoder : public core::QVDecoder {
   public:
    std::string decode(const core::parameter::QualityValues& param, const std::string& ecigar,
                       core::AccessUnitRaw::Descriptor& desc) override {
        std::string qv;
        const auto& param_casted = dynamic_cast<const quality::paramqv1::QualityValues1&>(param);
        if (!desc.get(0).end() && !desc.get(0).pull()) {
            return qv;
        }
        core::CigarTokenizer::tokenize(
            ecigar, core::getECigarInfo(),
            [&qv, &desc, &param_casted](uint8_t cigar, const util::StringView& bs, const util::StringView&) {
                uint8_t codebook = param_casted.getNumberCodeBooks() - 1;
                if (core::getECigarInfo().lut_step_ref[cigar] || core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar)) {
                    codebook = desc.get(1).end() ? 0 : desc.get(1).pull();
                }

                for(size_t i = 0; i < bs.length(); ++i) {
                    uint8_t index = desc.get(codebook + 2).pull();
                    qv.push_back(param_casted.getCodebook(codebook).getEntries()[index]);
                }
            });
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

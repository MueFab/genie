#ifndef GENIE_DECODER_H
#define GENIE_DECODER_H

#include <genie/core/cigar-tokenizer.h>
#include <genie/core/qv-decoder.h>
#include <genie/quality/paramqv1/qv_coding_config_1.h>
#include <genie/util/watch.h>

namespace genie {
namespace quality {
namespace qvwriteout {

class Decoder : public core::QVDecoder {
   public:
    std::tuple<std::vector<std::string>, core::stats::PerfStats> process(const core::parameter::QualityValues& param,
                                     const std::vector<std::string>& ecigar_vec,
                                     core::AccessUnit::Descriptor& desc) override {
        util::Watch watch;
        std::tuple<std::vector<std::string>, core::stats::PerfStats> qv;
        const auto& param_casted = dynamic_cast<const quality::paramqv1::QualityValues1&>(param);
        for (const auto& ecigar : ecigar_vec) {
            std::get<0>(qv).emplace_back();
            if (!desc.get(0).end() && !desc.get(0).pull()) {
                continue;
            }
            core::CigarTokenizer::tokenize(
                ecigar, core::getECigarInfo(),
                [&qv, &desc, &param_casted](uint8_t cigar, const util::StringView& bs, const util::StringView&) {
                    uint8_t codebook = param_casted.getNumberCodeBooks() - 1;
                    if (core::getECigarInfo().lut_step_ref[cigar] ||
                        core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar)) {
                        codebook = desc.get(1).end() ? 0 : desc.get(1).pull();
                    }

                    for (size_t i = 0; i < bs.length(); ++i) {
                        uint8_t index = desc.get(codebook + 2).pull();
                        std::get<0>(qv).back().push_back(param_casted.getCodebook(codebook).getEntries()[index]);
                    }
                });
        }
        std::get<1>(qv).addDouble("time-qv1writeout", watch.check());
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

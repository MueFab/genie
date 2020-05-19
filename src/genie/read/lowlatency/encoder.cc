/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"
#include <genie/name/tokenizer/decoder.h>
#include "genie/name/tokenizer/encoder.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace lowlatency {

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    core::record::Chunk data = std::move(t);

    core::parameter::ParameterSet set;
    LLState state{data.getData().front().getSegments().front().getSequence().length(),
                  data.getData().front().getNumberOfTemplateSegments() > 1,
                  core::AccessUnitRaw(std::move(set),
                                      data.getData().size() * data.getData().front().getNumberOfTemplateSegments())};
    for (auto& r : data.getData()) {
        for (auto& s : r.getSegments()) {
            state.streams.push(core::GenSub::RLEN, s.getSequence().length() - 1);
            if (state.readLength != s.getSequence().length()) {
                state.readLength = 0;
            }

            for (auto c : s.getSequence()) {
                state.streams.push(core::GenSub::UREADS,
                                   core::getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[c]);
            }
        }
        if (r.getSegments().size() > 1) {
            constexpr int16_t DUMMY_POS = 0;
            state.streams.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_SAME_RECORD);
            state.streams.push(core::GenSub::PAIR_SAME_REC, DUMMY_POS);
        }
    }

    core::QVEncoder::QVCoded qv(nullptr, core::AccessUnitRaw::Descriptor(core::GenDesc::QV));
    qv = qvcoder->process(data);
    core::AccessUnitRaw::Descriptor rname(core::GenDesc::RNAME);
    rname = namecoder->process(data);
    auto rawAU = pack(id, data.getData().front().getSegments().front().getQualities().size(), std::move(qv.first), state);

    rawAU.get(core::GenDesc::QV) = std::move(qv.second);
    rawAU.get(core::GenDesc::RNAME) = std::move(rname);

    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnitRaw Encoder::pack(const util::Section& id, uint8_t qv_depth,
                                  std::unique_ptr<core::parameter::QualityValues> qvparam, LLState& state) const {
    core::parameter::DataUnit::DatasetType dataType = core::parameter::DataUnit::DatasetType::NON_ALIGNED;
    core::parameter::ParameterSet ret(id.start, id.start, dataType, core::AlphabetID::ACGTN, state.readLength,
                                      state.pairedEnd, false, qv_depth, 0, false, false);
    ret.addClass(core::record::ClassType::CLASS_U, std::move(qvparam));

    auto rawAU = std::move(state.streams);

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(0);
    rawAU.setMinPos(0);
    rawAU.setMaxPos(0);
    rawAU.setClassType(core::record::ClassType::CLASS_U);

    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lowlatency
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace lowlatency {

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(core::QVEncoder* coder) : core::ReadEncoder(coder) {}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, size_t id)  {
    core::record::Chunk data = std::move(t);
    core::parameter::ParameterSet set;
    LLState state{data.front().getSegments().front().getSequence().length(),
                  data.front().getNumberOfTemplateSegments() > 1,
                  core::AccessUnitRaw(std::move(set), data.size() * data.front().getNumberOfTemplateSegments())};
    for (auto& r : data) {
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
    if (qvcoder) {
        qv = qvcoder->encode(data);
    }

    state.streams.get(core::GenDesc::QV) = std::move(qv.second);

    flowOut(pack(id, data.front().getSegments().front().getQualities().size(), std::move(qv.first), state), id);
}

void Encoder::dryIn()  { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnitRaw Encoder::pack(size_t id, uint8_t qv_depth, std::unique_ptr<core::parameter::QualityValues> qvparam,
                                  LLState& state) const {
    core::parameter::DataUnit::DatasetType dataType = core::parameter::DataUnit::DatasetType::ALIGNED;
    core::parameter::ParameterSet ret(id, id, dataType, core::AlphabetID::ACGTN, state.readLength, state.pairedEnd,
                                      false, qv_depth, 0, false, false);
    ret.addClass(core::record::ClassType::CLASS_U, std::move(qvparam));

    auto rawAU = std::move(state.streams);

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(0);
    rawAU.setMinPos(0);
    rawAU.setMaxPos(0);

    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lowlatency
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
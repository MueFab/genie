/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/lowlatency/encoder.h"
#include <memory>
#include <utility>
#include "genie/name/tokenizer/decoder.h"
#include "genie/name/tokenizer/encoder.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::lowlatency {

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    util::Watch watch;
    core::record::Chunk data = std::move(t);

    if (data.getData().empty()) {
        core::parameter::ParameterSet set;
        core::AccessUnit au(std::move(set.getEncodingSet()), 0);
        au.setReference(data.getRef(), data.getRefToWrite());
        au.setReference(uint16_t(data.getRefID()));
        flowOut(std::move(au), id);
        return;
    }

    core::parameter::ParameterSet set;

    LLState state{data.getData().front().getSegments().front().getSequence().length(),
                  data.getData().front().getNumberOfTemplateSegments() > 1,
                  core::AccessUnit(std::move(set.getEncodingSet()), data.getData().size()), data.isReferenceOnly()};
    size_t num_reads = 0;
    for (auto& r : data.getData()) {
        for (auto& s : r.getSegments()) {
            num_reads++;
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
            state.streams.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_SAME_RECORD);
        } else if (r.getNumberOfTemplateSegments() > 1) {
            if (r.getRead1First()) {
                state.streams.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R1_UNPAIRED);
            } else {
                state.streams.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R2_UNPAIRED);
            }
        }
    }
    watch.pause();

    auto qv = qvcoder->process(data);
    auto rname = namecoder->process(data);
    watch.resume();
    auto rawAU = pack(id, std::get<1>(qv).isEmpty() ? 0 : 1, std::move(std::get<0>(qv)), state);

    if (!data.isReferenceOnly()) {
        rawAU.get(core::GenDesc::QV) = std::move(std::get<1>(qv));
        rawAU.get(core::GenDesc::RNAME) = std::move(std::get<0>(rname));
    }
    if (state.readLength != 0) {
        rawAU.set(core::GenSub::RLEN, core::AccessUnit::Subsequence(core::GenSub::RLEN));
    }

    rawAU.setStats(std::move(data.getStats()));
    rawAU.getStats().addDouble("time-lowlatency", watch.check());
    rawAU.getStats().add(std::get<2>(qv));
    rawAU.getStats().add(std::get<1>(rname));
    rawAU = entropyCodeAU(std::move(rawAU));
    rawAU.setNumReads(num_reads);
    rawAU.setReferenceOnly(data.isReferenceOnly());
    rawAU.setReference(uint16_t(data.getRefID()));
    rawAU.setReference(data.getRef(), {});
    data.getData().clear();
    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit Encoder::pack(const util::Section& id, uint8_t qv_depth,
                               std::unique_ptr<core::parameter::QualityValues> qvparam, LLState& state) {
    core::parameter::DataUnit::DatasetType dataType = state.refOnly
                                                          ? core::parameter::DataUnit::DatasetType::REFERENCE
                                                          : core::parameter::DataUnit::DatasetType::NON_ALIGNED;
    core::parameter::ParameterSet ret(uint8_t(id.start), uint8_t(id.start), dataType, core::AlphabetID::ACGTN,
                                      uint32_t(state.readLength), state.pairedEnd, false, qv_depth, 0, false, false);
    ret.getEncodingSet().addClass(core::record::ClassType::CLASS_U, std::move(qvparam));

    auto rawAU = std::move(state.streams);

    rawAU.setParameters(std::move(ret.getEncodingSet()));
    rawAU.setReference(0);
    rawAU.setMinPos(0);
    rawAU.setMaxPos(0);
    rawAU.setClassType(core::record::ClassType::CLASS_U);

    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::lowlatency

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

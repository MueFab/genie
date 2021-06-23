/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/refcoder/encoder.h"
#include <memory>
#include <string>
#include <utility>
#include "genie/read/basecoder/encoder.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace refcoder {

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder() : core::ReadEncoder() {}

// ---------------------------------------------------------------------------------------------------------------------

const core::record::alignment_split::SameRec& Encoder::getPairedAlignment(const core::record::Record& r) {
    UTILS_DIE_IF(r.getAlignments().front().getAlignmentSplits().front()->getType() !=
                     core::record::AlignmentSplit::Type::SAME_REC,
                 "Only same record split alignments supported");
    return *reinterpret_cast<const core::record::alignment_split::SameRec*>(
        r.getAlignments().front().getAlignmentSplits().front().get());
}

void Encoder::updateAssembly(const core::record::Record& r, State& state,
                             const core::ReferenceManager::ReferenceExcerpt& excerpt) const {
    std::string ref1 = excerpt.getString(
        r.getAlignments().front().getPosition(),
        r.getAlignments().front().getPosition() +
            core::record::Record::getLengthOfCigar(r.getAlignments().front().getAlignment().getECigar()));
    std::string ref2;

    if (r.getNumberOfTemplateSegments() > 1) {
        const auto& srec = getPairedAlignment(r);
        ref2 = excerpt.getString(r.getAlignments().front().getPosition() + srec.getDelta(),
                                 r.getAlignments().front().getPosition() + srec.getDelta() +
                                     core::record::Record::getLengthOfCigar(srec.getAlignment().getECigar()));
    }

    state.readCoder.add(r, ref1, ref2);
}

core::AccessUnit Encoder::pack(size_t id, uint16_t ref, uint8_t qv_depth,
                               std::unique_ptr<core::parameter::QualityValues> qvparam, core::record::ClassType type,
                               State& state) const {
    core::parameter::DataUnit::DatasetType dataType = core::parameter::DataUnit::DatasetType::ALIGNED;
    core::parameter::ParameterSet ret((uint8_t)id, (uint8_t)id, dataType, core::AlphabetID::ACGTN,
                                      uint32_t(state.readLength), state.pairedEnd, false, qv_depth, 0, false, false);
    ret.addClass(type, std::move(qvparam));

    auto rawAU = state.readCoder.moveStreams();

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(ref);
    rawAU.setMinPos(state.minPos);
    rawAU.setMaxPos(state.maxPos);
    rawAU.setClassType(type);
    return rawAU;
}

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    util::Watch watch;
    core::record::Chunk data = std::move(t);

    State state(data.getRef().getGlobalStart());
    state.minPos = data.getRef().getGlobalStart();
    state.maxPos = data.getRef().getGlobalEnd();
    state.pairedEnd = data.getData().front().getNumberOfTemplateSegments() > 1;
    state.readLength = data.getData().front().getSegments().front().getSequence().length();

    auto ref = data.getData().front().getAlignmentSharedData().getSeqID();
    uint64_t lastPos = 0;

    auto qv = qvcoder->process(data);
    uint8_t qvdepth = std::get<1>(qv).isEmpty() ? 0 : 1;

    core::AccessUnit::Descriptor rname(core::GenDesc::RNAME);
    rname = std::get<0>(namecoder->process(data));

    size_t read_num = 0;
    for (auto& r : data.getData()) {
        read_num += r.getSegments().size();
        UTILS_DIE_IF(r.getAlignments().front().getPosition() < lastPos,
                     "Data seems to be unsorted. Reference based encoding needs sorted input data.");

        UTILS_DIE_IF((r.getNumberOfTemplateSegments() > 1) != state.pairedEnd,
                     "Mixed paired / single end records not supported.");

        for (const auto& s : r.getSegments()) {
            if (s.getSequence().length() != state.readLength) {
                state.readLength = 0;
            }
        }

        lastPos = r.getAlignments().front().getPosition();

        updateAssembly(r, state, data.getRef());
    }

    auto rawAU = pack(id.start, ref, qvdepth, std::move(std::get<0>(qv)), data.getData().front().getClassID(), state);
    rawAU.get(core::GenDesc::QV) = std::move(std::get<1>(qv));
    rawAU.get(core::GenDesc::RNAME) = std::move(rname);
    rawAU.get(core::GenDesc::FLAGS) = core::AccessUnit::Descriptor(core::GenDesc::FLAGS);
    rawAU.setStats(std::move(data.getStats()));
    data.getData().clear();
    rawAU = entropyCodeAU(std::move(rawAU));
    rawAU.setNumReads(read_num);
    rawAU.setReference(data.getRef(), data.getRefToWrite());
    rawAU.setReference(uint16_t(data.getRefID()));
    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

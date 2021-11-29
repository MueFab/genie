/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/refcoder/encoder.h"
#include <algorithm>
#include <limits>
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

Encoder::EncodingState::EncodingState(uint64_t startingPos)
    : readCoder((int32_t)startingPos),
      pairedEnd(false),
      readLength(0),
      classType(core::record::ClassType::NONE),
      minPos(std::numeric_limits<uint64_t>::max()),
      maxPos(std::numeric_limits<uint64_t>::min()) {}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::updateAssembly(const core::record::Record& r, EncodingState& state,
                             const core::ReferenceManager::ReferenceExcerpt& excerpt) const {
    std::string ref1 = excerpt.getString(
        r.getAlignments().front().getPosition(),
        r.getAlignments().front().getPosition() +
            core::record::Record::getLengthOfCigar(r.getAlignments().front().getAlignment().getECigar()));
    std::string ref2;

    if (r.getSegments().size() > 1) {
        const auto& srec = getPairedAlignment(state, r);
        ref2 = excerpt.getString(r.getAlignments().front().getPosition() + srec.getDelta(),
                                 r.getAlignments().front().getPosition() + srec.getDelta() +
                                     core::record::Record::getLengthOfCigar(srec.getAlignment().getECigar()));
    }

    state.readCoder.add(r, ref1, ref2);
}

// ---------------------------------------------------------------------------------------------------------------------

const core::record::alignment_split::SameRec& Encoder::getPairedAlignment(const Encoder::EncodingState& state,
                                                                          const core::record::Record& r) const {
    UTILS_DIE_IF(!state.pairedEnd, "Record is single ended");
    UTILS_DIE_IF(r.getAlignments().front().getAlignmentSplits().front()->getType() !=
                     core::record::AlignmentSplit::Type::SAME_REC,
                 "Only same record split alignments supported");
    return *reinterpret_cast<const core::record::alignment_split::SameRec*>(
        r.getAlignments().front().getAlignmentSplits().front().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::updateGuesses(const core::record::Record& r, Encoder::EncodingState& state) const {
    UTILS_DIE_IF((r.getNumberOfTemplateSegments() > 1) != state.pairedEnd, "Mix of paired / unpaired not supported");

    state.classType = std::max(state.classType, r.getClassID());

    if (r.getSegments()[0].getSequence().length() != state.readLength) {
        state.readLength = 0;
    }

    if (r.getSegments().size() == 2) {
        if (r.getSegments()[1].getSequence().length() != state.readLength) {
            state.readLength = 0;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit Encoder::pack(size_t id, uint16_t ref, uint8_t qv_depth,
                               std::unique_ptr<core::parameter::QualityValues> qvparam, core::record::ClassType type,
                               Encoder::EncodingState& state) const {
    core::parameter::DataUnit::DatasetType dataType = core::parameter::DataUnit::DatasetType::ALIGNED;
    core::parameter::ParameterSet ret((uint8_t)id, (uint8_t)id, dataType, core::AlphabetID::ACGTN,
                                      (uint32_t)state.readLength, state.pairedEnd, false, qv_depth, 0, false, false);
    ret.addClass(type, std::move(qvparam));

    auto rawAU = state.readCoder.moveStreams();

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(ref);
    rawAU.setMinPos(state.minPos);
    rawAU.setMaxPos(state.maxPos);
    rawAU.setClassType(type);
    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    util::Watch watch;
    core::record::Chunk data = std::move(t);
    if (data.getData().empty()) {
        skipOut(id);
        return;
    }

    EncodingState state(data.getRef().getGlobalStart());
    state.minPos = data.getRef().getGlobalStart();
    state.maxPos = data.getRef().getGlobalEnd();

    state.pairedEnd = data.getData().front().getNumberOfTemplateSegments() > 1;
    state.readLength = data.getData().front().getSegments().front().getSequence().length();

    auto ref = data.getData().front().getAlignmentSharedData().getSeqID();
    uint64_t lastPos = 0;
    data.getStats().addDouble("time-localassembly", watch.check());
    watch.reset();
    auto qv = qvcoder->process(data);
    uint8_t qvdepth = std::get<1>(qv).isEmpty() ? 0 : 1;
    data.getStats().addDouble("time-quality", watch.check());
    watch.reset();
    core::AccessUnit::Descriptor rname(core::GenDesc::RNAME);
    rname = std::get<0>(namecoder->process(data));
    data.getStats().addDouble("time-name", watch.check());
    watch.reset();
    size_t read_num = 0;
    for (auto& r : data.getData()) {
        read_num += r.getSegments().size();
        uint64_t curPos = r.getAlignments().front().getPosition();
        UTILS_DIE_IF(curPos < lastPos, "Data seems to be unsorted. Local assembly encoding needs sorted input data.");

        lastPos = r.getAlignments().front().getPosition();

        updateGuesses(r, state);

        UTILS_DIE_IF(r.getAlignmentSharedData().getSeqID() != ref,
                     "Records belonging to different reference sequences in one access unit");

        updateAssembly(r, state, data.getRef());
    }

    auto rawAU = pack(id.start, ref, qvdepth, std::move(std::get<0>(qv)), data.getData().front().getClassID(), state);
    rawAU.get(core::GenDesc::QV) = std::move(std::get<1>(qv));
    rawAU.get(core::GenDesc::RNAME) = std::move(rname);
    if (data.getData().front().getClassID() < core::record::ClassType::CLASS_I) {
        rawAU.get(core::GenSub::CLIPS_HARD_LENGTH) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_HARD_LENGTH);
        rawAU.get(core::GenSub::CLIPS_TYPE) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_TYPE);
        rawAU.get(core::GenSub::CLIPS_SOFT_STRING) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_SOFT_STRING);
        rawAU.get(core::GenSub::CLIPS_RECORD_ID) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_RECORD_ID);
        rawAU.get(core::GenSub::MMTYPE_INSERTION) = core::AccessUnit::Subsequence(core::GenSub::MMTYPE_INSERTION);
        rawAU.get(core::GenSub::MMTYPE_TYPE) = core::AccessUnit::Subsequence(core::GenSub::MMTYPE_TYPE);
    }
    if (data.getData().front().getClassID() < core::record::ClassType::CLASS_M) {
        rawAU.get(core::GenSub::MMTYPE_SUBSTITUTION) = core::AccessUnit::Subsequence(core::GenSub::MMTYPE_SUBSTITUTION);
    }
    if (data.getData().front().getClassID() < core::record::ClassType::CLASS_N) {
        rawAU.get(core::GenSub::MMPOS_TERMINATOR) = core::AccessUnit::Subsequence(core::GenSub::MMPOS_TERMINATOR);
        rawAU.get(core::GenSub::MMPOS_POSITION) = core::AccessUnit::Subsequence(core::GenSub::MMPOS_POSITION);
    }
    rawAU.setStats(std::move(data.getStats()));
    data.getData().clear();
    rawAU.getStats().addDouble("time-localassembly", watch.check());
    rawAU = entropyCodeAU(std::move(rawAU));
    rawAU.setNumReads(read_num);
    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

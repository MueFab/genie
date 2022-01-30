/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/basecoder/encoderstub.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

// ---------------------------------------------------------------------------------------------------------------------

EncoderStub::EncodingState::EncodingState(const core::record::Chunk& data)
    : readCoder(data.getData().front().getAlignments().front().getPosition()),
      pairedEnd(data.getData().front().getNumberOfTemplateSegments() > 1),
      readLength(data.getData().front().getSegments().front().getSequence().length()),
      classType(data.getData().front().getClassID()),
      minPos(data.getRef().getGlobalStart()),
      maxPos(data.getRef().getGlobalEnd()),
      ref(data.getData().front().getAlignmentSharedData().getSeqID()),
      readNum(0),
      lastReadPosition(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void EncoderStub::encodeSeq(core::record::Chunk& data, EncodingState& state) {
    util::Watch watch;
    for (auto& r : data.getData()) {
        state.readNum += r.getSegments().size();
        uint64_t curPos = r.getAlignments().front().getPosition();
        UTILS_DIE_IF(curPos < state.lastReadPosition,
                     "Data seems to be unsorted. Local assembly encoding needs sorted input data.");

        state.lastReadPosition = curPos;

        // Check if length constant
        for (const auto& s : r.getSegments()) {
            if (s.getSequence().length() != state.readLength) {
                state.readLength = 0;
            }
        }

        UTILS_DIE_IF(r.getAlignmentSharedData().getSeqID() != state.ref,
                     "Records belonging to different reference sequences in one access unit");

        auto refs = getReferences(r, state);
        state.readCoder.add(r, refs.first, refs.second);
    }
    data.getStats().addDouble("time-basecoder", watch.check());
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit EncoderStub::pack(size_t id, core::QVEncoder::QVCoded qv, core::AccessUnit::Descriptor rname,
                                   EncodingState& state) {
    core::parameter::DataUnit::DatasetType dataType = core::parameter::DataUnit::DatasetType::ALIGNED;
    auto qv_depth = static_cast<uint8_t>(std::get<1>(qv).isEmpty() ? 0 : 1);
    core::parameter::ParameterSet ret((uint8_t)id, (uint8_t)id, dataType, core::AlphabetID::ACGTN,
                                      (uint32_t)state.readLength, state.pairedEnd, false, qv_depth, uint8_t(0), false,
                                      false);
    ret.getEncodingSet().addClass(state.classType, std::move(std::get<0>(qv)));

    auto rawAU = state.readCoder.moveStreams();

    rawAU.get(core::GenDesc::QV) = std::move(std::get<1>(qv));
    rawAU.get(core::GenDesc::RNAME) = std::move(rname);

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(state.ref);
    rawAU.setMinPos(state.minPos);
    rawAU.setMaxPos(state.maxPos);
    rawAU.setClassType(state.classType);
    rawAU.setNumReads(state.readNum);

    removeRedundantDescriptors(rawAU);
    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

void EncoderStub::removeRedundantDescriptors(core::AccessUnit& rawAU) {
    if (rawAU.getClassType() < core::record::ClassType::CLASS_I) {
        rawAU.get(core::GenSub::CLIPS_HARD_LENGTH) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_HARD_LENGTH);
        rawAU.get(core::GenSub::CLIPS_TYPE) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_TYPE);
        rawAU.get(core::GenSub::CLIPS_SOFT_STRING) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_SOFT_STRING);
        rawAU.get(core::GenSub::CLIPS_RECORD_ID) = core::AccessUnit::Subsequence(core::GenSub::CLIPS_RECORD_ID);
        rawAU.get(core::GenSub::MMTYPE_INSERTION) = core::AccessUnit::Subsequence(core::GenSub::MMTYPE_INSERTION);
        rawAU.get(core::GenSub::MMTYPE_TYPE) = core::AccessUnit::Subsequence(core::GenSub::MMTYPE_TYPE);
    }
    if (rawAU.getClassType() < core::record::ClassType::CLASS_M) {
        rawAU.get(core::GenSub::MMTYPE_SUBSTITUTION) = core::AccessUnit::Subsequence(core::GenSub::MMTYPE_SUBSTITUTION);
    }
    if (rawAU.getClassType() < core::record::ClassType::CLASS_N) {
        rawAU.get(core::GenSub::MMPOS_TERMINATOR) = core::AccessUnit::Subsequence(core::GenSub::MMPOS_TERMINATOR);
        rawAU.get(core::GenSub::MMPOS_POSITION) = core::AccessUnit::Subsequence(core::GenSub::MMPOS_POSITION);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::QVEncoder::QVCoded EncoderStub::encodeQVs(QvSelector* qvcoder, core::record::Chunk& data) {
    util::Watch watch;
    auto qv = qvcoder->process(data);
    data.getStats().addDouble("time-quality", watch.check());
    return qv;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Descriptor EncoderStub::encodeNames(NameSelector* namecoder, core::record::Chunk& data) {
    util::Watch watch;
    auto name = namecoder->process(data);
    data.getStats().addDouble("time-name", watch.check());
    return std::get<0>(name);
}

// ---------------------------------------------------------------------------------------------------------------------

void EncoderStub::flowIn(core::record::Chunk&& t, const util::Section& id) {
    core::record::Chunk data = std::move(t);

    // Empty block: do nothing
    if (data.getData().empty()) {
        skipOut(id);
        return;
    }

    auto qv = encodeQVs(qvcoder, data);
    auto rname = encodeNames(namecoder, data);

    auto state = createState(data);
    encodeSeq(data, *state);

    auto rawAU = pack(id.start, std::move(qv), std::move(rname), *state);
    rawAU.setStats(std::move(data.getStats()));
    data.getData().clear();
    rawAU = entropyCodeAU(std::move(rawAU));
    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

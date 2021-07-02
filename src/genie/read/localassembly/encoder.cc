/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/localassembly/encoder.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

// ---------------------------------------------------------------------------------------------------------------------

Encoder::LaeState::LaeState(size_t cr_buf_max_size, uint64_t startingPos)
    : refCoder((uint32_t)cr_buf_max_size),
      readCoder((int32_t)startingPos),
      pairedEnd(false),
      readLength(0),
      classType(core::record::ClassType::NONE),
      minPos(std::numeric_limits<uint64_t>::max()),
      maxPos(std::numeric_limits<uint64_t>::min()) {}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::printDebug(const Encoder::LaeState& state, const std::string& ref1, const std::string& ref2,
                         const core::record::Record& r) const {
    if (!debug) {
        return;
    }
    state.refCoder.printWindow();
    std::cout << "pair!" << std::endl;
    std::cout << "ref1: " << std::endl;
    for (size_t i = 0; i < r.getAlignments().front().getPosition() - state.refCoder.getWindowBorder(); ++i) {
        std::cout << " ";
    }
    std::cout << ref1 << std::endl;

    std::cout << "ref2: " << std::endl;
    for (size_t i = 0; i < r.getAlignments().front().getPosition() - state.refCoder.getWindowBorder(); ++i) {
        std::cout << " ";
    }
    std::cout << ref2 << std::endl;
    std::cout << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::updateAssembly(const core::record::Record& r, Encoder::LaeState& state) const {
    std::string ref1 = state.refCoder.getReference((uint32_t)r.getAlignments().front().getPosition(),
                                                   r.getAlignments().front().getAlignment().getECigar());
    std::string ref2;

    updateAUBoundaries(r.getAlignments().front().getPosition(), r.getAlignments().front().getAlignment().getECigar(),
                       state);
    if (state.pairedEnd && r.getAlignments().front().getAlignmentSplits().front()->getType() ==
                               core::record::AlignmentSplit::Type::SAME_REC) {
        const auto& srec = getPairedAlignment(state, r);
        ref2 = state.refCoder.getReference((uint32_t)(r.getAlignments().front().getPosition() + srec.getDelta()),
                                           srec.getAlignment().getECigar());
        updateAUBoundaries(r.getAlignments().front().getPosition() + srec.getDelta(), srec.getAlignment().getECigar(),
                           state);
    }

    state.refCoder.addRead(r);
    state.readCoder.add(r, ref1, ref2);

    printDebug(state, ref1, ref2, r);
}

// ---------------------------------------------------------------------------------------------------------------------

const core::record::alignment_split::SameRec& Encoder::getPairedAlignment(const Encoder::LaeState& state,
                                                                          const core::record::Record& r) const {
    UTILS_DIE_IF(!state.pairedEnd, "Record is single ended");
    UTILS_DIE_IF(r.getAlignments().front().getAlignmentSplits().front()->getType() !=
                     core::record::AlignmentSplit::Type::SAME_REC,
                 "Only same record split alignments supported");
    return *reinterpret_cast<const core::record::alignment_split::SameRec*>(
        r.getAlignments().front().getAlignmentSplits().front().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::updateGuesses(const core::record::Record& r, Encoder::LaeState& state) const {
    UTILS_DIE_IF((r.getNumberOfTemplateSegments() > 1) != state.pairedEnd, "Mix of paired / unpaired not supported");

    state.classType = std::max(state.classType, r.getClassID());

    if (r.getSegments()[0].getSequence().length() != state.readLength) {
        state.readLength = 0;
    }

    if (state.pairedEnd) {
        if (r.getSegments()[1].getSequence().length() != state.readLength) {
            state.readLength = 0;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit Encoder::pack(size_t id, uint16_t ref, uint8_t qv_depth,
                               std::unique_ptr<core::parameter::QualityValues> qvparam, core::record::ClassType type,
                               Encoder::LaeState& state) const {
    core::parameter::DataUnit::DatasetType dataType = core::parameter::DataUnit::DatasetType::ALIGNED;
    core::parameter::ParameterSet ret((uint8_t)id, (uint8_t)id, dataType, core::AlphabetID::ACGTN,
                                      (uint32_t)state.readLength, state.pairedEnd, false, qv_depth, 0, false, false);
    ret.addClass(type, std::move(qvparam));
    auto crps = core::parameter::ComputedRef(core::parameter::ComputedRef::Algorithm::LOCAL_ASSEMBLY);
    crps.setExtension(core::parameter::ComputedRefExtended(0, cr_buf_max_size));
    ret.setComputedRef(std::move(crps));

    auto rawAU = state.readCoder.moveStreams();

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(ref);
    rawAU.setMinPos(1);
    rawAU.setMaxPos(state.maxPos - state.minPos + 1);
    rawAU.setClassType(type);
    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    util::Watch watch;
    core::record::Chunk data = std::move(t);
    LaeState state(cr_buf_max_size, data.getData().front().getAlignments().front().getPosition());

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
        if (r.getClassID() == core::record::ClassType::CLASS_HM) {
            std::cerr << "Skipping record of class HM in local assembly..." << std::endl;
            continue;
        }
        read_num += r.getSegments().size();
        uint64_t curPos = (!r.isRead1First()) && r.getAlignments().front().getAlignmentSplits().front()->getType() ==
                                                     core::record::AlignmentSplit::Type::SAME_REC
                              ? r.getAlignments().front().getPosition() + getPairedAlignment(state, r).getDelta()
                              : r.getAlignments().front().getPosition();
        UTILS_DIE_IF(curPos < lastPos, "Data seems to be unsorted. Local assembly encoding needs sorted input data.");

        lastPos = r.getAlignments().front().getPosition();

        updateGuesses(r, state);

        UTILS_DIE_IF(r.getAlignmentSharedData().getSeqID() != ref,
                     "Records belonging to different reference sequences in one access unit");

        updateAssembly(r, state);
    }

    auto rawAU = pack(id.start, ref, qvdepth, std::move(std::get<0>(qv)), data.getData().front().getClassID(), state);
    rawAU.get(core::GenDesc::QV) = std::move(std::get<1>(qv));
    rawAU.get(core::GenDesc::RNAME) = std::move(rname);
    rawAU.get(core::GenDesc::FLAGS) = core::AccessUnit::Descriptor(core::GenDesc::FLAGS);
    rawAU.setStats(std::move(data.getStats()));
    data.getData().clear();
    rawAU.getStats().addDouble("time-localassembly", watch.check());
    rawAU = entropyCodeAU(std::move(rawAU));
    rawAU.setNumReads(read_num);
    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(uint32_t _cr_buf_max_size, bool _debug) : debug(_debug), cr_buf_max_size(_cr_buf_max_size) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Encoder::getLengthOfCigar(const std::string& cigar) {
    std::string digits;
    size_t length = 0;
    for (const auto& c : cigar) {
        if (isdigit(c)) {
            digits += c;
            continue;
        }
        if (getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(c)) {
            length++;
            digits.clear();
            continue;
        }
        if (c == '=' || c == '-' || c == '*' || c == '/' || c == '%') {
            length += std::stoi(digits);
            digits.clear();
        } else {
            digits.clear();
        }
    }
    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::updateAUBoundaries(uint64_t position, const std::string& cigar, LaeState& state) const {
    state.minPos = std::min(position, state.minPos);
    state.maxPos = std::max(position + getLengthOfCigar(cigar), state.maxPos);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

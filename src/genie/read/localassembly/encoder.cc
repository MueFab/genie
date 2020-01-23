/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"
#include <genie/quality/paramqv1/qv_coding_config_1.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

// ---------------------------------------------------------------------------------------------------------------------

Encoder::LaeState::LaeState(size_t cr_buf_max_size)
    : refCoder(cr_buf_max_size),
      readCoder(),
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
    std::string ref1 = state.refCoder.getReference(r.getAlignments().front().getPosition(),
                                                   r.getAlignments().front().getAlignment().getECigar());
    std::string ref2;

    updateAUBoundaries(r.getAlignments().front().getPosition(), r.getAlignments().front().getAlignment().getECigar(),
                       state);
    if (state.pairedEnd) {
        const auto& srec = getPairedAlignment(state, r);
        ref2 = state.refCoder.getReference(r.getAlignments().front().getPosition() + srec.getDelta(),
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

core::AccessUnitRaw Encoder::pack(size_t id, uint16_t ref, Encoder::LaeState& state) const {
    core::parameter::DataUnit::DatasetType dataType = core::parameter::DataUnit::DatasetType::ALIGNED;
    core::parameter::ParameterSet ret(id, id, dataType, core::AlphabetID::ACGTN, state.readLength, state.pairedEnd,
                                      false, 0, 0, false, false);
    const auto ALPHABET = quality::paramqv1::QualityValues1::QvpsPresetId::ASCII;
    ret.addClass(state.classType, util::make_unique<quality::paramqv1::QualityValues1>(ALPHABET, false));
    auto crps =
        util::make_unique<core::parameter::ComputedRef>(core::parameter::ComputedRef::Algorithm::LOCAL_ASSEMBLY);
    crps->setExtension(util::make_unique<core::parameter::ComputedRefExtended>(0, cr_buf_max_size));
    ret.setComputedRef(std::move(crps));

    auto rawAU = state.readCoder.moveStreams();

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(ref);
    rawAU.setMinPos(state.minPos);
    rawAU.setMaxPos(state.maxPos);

    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, size_t id) {
    core::record::Chunk data = std::move(t);
    LaeState state(cr_buf_max_size);

    state.pairedEnd = data.front().getNumberOfTemplateSegments() > 1;
    state.readLength = data.front().getSegments().front().getSequence().length();

    auto ref = data.front().getAlignmentSharedData().getSeqID();
    uint64_t lastPos = 0;
    for (auto& r : data) {
        UTILS_DIE_IF(r.getAlignments().front().getPosition() < lastPos,
                     "Data seems to be unsorted. Local assembly encoding needs sorted input data.");

        lastPos = r.getAlignments().front().getPosition();

        updateGuesses(r, state);

        UTILS_DIE_IF(r.getAlignmentSharedData().getSeqID() != ref,
                     "Records belonging to different reference sequences in one access unit");

        updateAssembly(r, state);
    }

    auto rawAU = pack(id, ref, state);

    data.clear();
    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::dryIn() {
    // Output data left over
    dryOut();
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
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "local-assembly-encoder.h"
#include <format/mpegg_p2/parameter_set/qv_coding_config_1/qv_coding_config_1.h>
#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/segment.h>
#include <format/mpegg_rec/split_alignment/split-alignment-same-rec.h>
#include <util/make_unique.h>

namespace coding {

// ---------------------------------------------------------------------------------------------------------------------

LocalAssemblyEncoder::LaeState::LaeState(size_t cr_buf_max_size)
    : refCoder(cr_buf_max_size),
      readCoder(),
      pairedEnd(false),
      readLength(0),
      classType(format::mpegg_rec::ClassType::NONE),
      minPos(std::numeric_limits<uint64_t>::max()),
      maxPos(std::numeric_limits<uint64_t>::min()) {}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::printDebug(const LocalAssemblyEncoder::LaeState& state, const std::string& ref1,
                                      const std::string& ref2, const format::mpegg_rec::MpeggRecord& r) const {
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

void LocalAssemblyEncoder::updateAssembly(const format::mpegg_rec::MpeggRecord& r,
                                          LocalAssemblyEncoder::LaeState& state) const {
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

const format::mpegg_rec::SplitAlignmentSameRec& LocalAssemblyEncoder::getPairedAlignment(
    const LocalAssemblyEncoder::LaeState& state, const format::mpegg_rec::MpeggRecord& r) const {
    if (!state.pairedEnd) {
        UTILS_DIE("Record is single ended");
    }
    if (r.getAlignments().front().getSplitAlignments().front()->getType() ==
        format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
        return *reinterpret_cast<const format::mpegg_rec::SplitAlignmentSameRec*>(
            r.getAlignments().front().getSplitAlignments().front().get());
    } else {
        UTILS_DIE("Only same record split alignments supported");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::updateGuesses(const format::mpegg_rec::MpeggRecord& r,
                                         LocalAssemblyEncoder::LaeState& state) const {
    if ((r.getNumberOfTemplateSegments() > 1) != state.pairedEnd) {
        UTILS_DIE("Mix of paired / unpaired not supported");
    }

    state.classType = std::max(state.classType, r.getClassID());

    if (r.getRecordSegments()[0].getSequence().length() != state.readLength) {
        state.readLength = 0;
    }

    if (state.pairedEnd) {
        if (r.getRecordSegments()[1].getSequence().length() != state.readLength) {
            state.readLength = 0;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu LocalAssemblyEncoder::pack(size_t id, uint16_t ref, LocalAssemblyEncoder::LaeState& state) const {
    format::mpegg_p2::DataUnit::DatasetType dataType = format::mpegg_p2::DataUnit::DatasetType::ALIGNED;
    format::mpegg_p2::ParameterSet ret(id, id, dataType, AlphabetID::ACGTN, state.readLength, state.pairedEnd, false, 0,
                                       0, false, false);
    const auto ALPHABET = format::mpegg_p2::qv_coding1::QvCodingConfig1::QvpsPresetId::ASCII;
    ret.addClass(state.classType, util::make_unique<format::mpegg_p2::qv_coding1::QvCodingConfig1>(ALPHABET, false));
    auto crps = util::make_unique<format::mpegg_p2::ParameterSetCrps>(
        format::mpegg_p2::ParameterSetCrps::CrAlgId::LOCAL_ASSEMBLY);
    crps->setCrpsInfo(util::make_unique<format::mpegg_p2::CrpsInfo>(0, cr_buf_max_size));
    ret.setCrps(std::move(crps));

    auto rawAU = state.readCoder.moveStreams();

    rawAU.setParameters(std::move(ret));
    rawAU.setReference(ref);
    rawAU.setMinPos(state.minPos);
    rawAU.setMaxPos(state.maxPos);

    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::flowIn(format::mpegg_rec::MpeggChunk&& t, size_t id) {
    format::mpegg_rec::MpeggChunk data = std::move(t);
    LaeState state(cr_buf_max_size);

    state.pairedEnd = data.front().getNumberOfTemplateSegments() > 1;
    state.readLength = data.front().getRecordSegments().front().getSequence().length();

    auto ref = data.front().getMetaAlignment().getSeqID();
    uint64_t lastPos = 0;
    for (auto& r : data) {
        if (r.getAlignments().front().getPosition() < lastPos) {
            UTILS_DIE("Data seems to be unsorted. Local assembly encoding needs sorted input data.");
        }

        lastPos = r.getAlignments().front().getPosition();

        updateGuesses(r, state);

        if (r.getMetaAlignment().getSeqID() != ref) {
            UTILS_DIE("Records belonging to different reference sequences in one access unit");
        }

        updateAssembly(r, state);
    }

    auto rawAU = pack(id, ref, state);

    data.clear();
    flowOut(std::move(rawAU), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::dryIn() {
    // Output data left over
    dryOut();
}

// ---------------------------------------------------------------------------------------------------------------------

LocalAssemblyEncoder::LocalAssemblyEncoder(uint32_t _cr_buf_max_size, bool _debug)
    : debug(_debug), cr_buf_max_size(_cr_buf_max_size) {}

}  // namespace coding

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
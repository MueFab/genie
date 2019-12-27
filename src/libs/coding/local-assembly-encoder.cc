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

// ---------------------------------------------------------------------------------------------------------------------

LocalAssemblyEncoder::LaeState::LaeState(size_t cr_buf_max_size)
    : refCoder(cr_buf_max_size),
      readCoder(),
      pairedEnd(false),
      readLength(0),
      classType(format::mpegg_rec::MpeggRecord::ClassType::NONE) {}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::printDebug(const LocalAssemblyEncoder::LaeState& state, const std::string& ref1,
                                      const std::string& ref2, const format::mpegg_rec::MpeggRecord& r) const {
    if (!debug) {
        return;
    }
    state.refCoder.printWindow();
    std::cout << "pair!" << std::endl;
    std::cout << "ref1: " << std::endl;
    for (size_t i = 0; i < r.getAlignment(0)->getPosition() - state.refCoder.getWindowBorder(); ++i) {
        std::cout << " ";
    }
    std::cout << ref1 << std::endl;

    std::cout << "ref2: " << std::endl;
    for (size_t i = 0; i < r.getAlignment(0)->getPosition() - state.refCoder.getWindowBorder(); ++i) {
        std::cout << " ";
    }
    std::cout << ref2 << std::endl;
    std::cout << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::updateAssembly(const format::mpegg_rec::MpeggRecord& r,
                                          const format::mpegg_rec::SplitAlignmentSameRec& srec,
                                          LocalAssemblyEncoder::LaeState* state) const {
    std::string ref1 =
        state->refCoder.getReference(r.getAlignment(0)->getPosition(), *r.getAlignment(0)->getAlignment()->getECigar());
    std::string ref2;
    if (state->pairedEnd) {
        ref2 = state->refCoder.getReference(r.getAlignment(0)->getPosition() + srec.getDelta(),
                                            *srec.getAlignment()->getECigar());
    }

    state->refCoder.addRead(&r);
    state->readCoder.add(&r, ref1, ref2);

    printDebug(*state, ref1, ref2, r);
}

// ---------------------------------------------------------------------------------------------------------------------

const format::mpegg_rec::SplitAlignmentSameRec* LocalAssemblyEncoder::getPairedAlignment(
    const LocalAssemblyEncoder::LaeState& state, const format::mpegg_rec::MpeggRecord& r) const {
    if (!state.pairedEnd) {
        return nullptr;
    }
    if (r.getAlignment(0)->getSplitAlignment(0)->getType() ==
        format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
        return reinterpret_cast<const format::mpegg_rec::SplitAlignmentSameRec*>(
            r.getAlignment(0)->getSplitAlignment(0));
    } else {
        UTILS_DIE("Only same record split alignments supported");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::updateGuesses(LocalAssemblyEncoder::LaeState* state,
                                        const format::mpegg_rec::MpeggRecord& r) const {
    if ((r.getNumberOfTemplateSegments() > 1) != state->pairedEnd) {
        UTILS_DIE("Mix of paired / unpaired not supported");
    }

    state->classType = std::max(state->classType, r.getClassID());

    if (r.getRecordSegment(0)->getLength() != state->readLength) {
        state->readLength = 0;
    }

    if (state->pairedEnd) {
        if (r.getRecordSegment(1)->getLength() != state->readLength) {
            state->readLength = 0;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<MpeggRawAu> LocalAssemblyEncoder::pack(size_t id, LocalAssemblyEncoder::LaeState* state) const {
    format::mpegg_p2::DataUnit::DatasetType dataType = format::mpegg_p2::DataUnit::DatasetType::ALIGNED;
    auto ret = util::make_unique<format::mpegg_p2::ParameterSet>(id, id, dataType, AlphabetID::ACGTN, state->readLength,
                                                                 state->pairedEnd, false, 0, 0, false, false);
    const auto ALPHABET = format::mpegg_p2::qv_coding1::QvCodingConfig1::QvpsPresetId::ASCII;
    ret->addClass(state->classType, util::make_unique<format::mpegg_p2::qv_coding1::QvCodingConfig1>(ALPHABET, false));

    auto rawAU = state->readCoder.moveStreams();

    rawAU->setParameters(std::move(ret));

    return rawAU;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyEncoder::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    LaeState state(cr_buf_max_size);

    state.pairedEnd = t->front()->getNumberOfTemplateSegments() > 1;
    state.readLength = t->front()->getRecordSegment(0)->getLength();

    for (const auto& r : *t) {
        updateGuesses(&state, *r);

        const format::mpegg_rec::SplitAlignmentSameRec* srec = getPairedAlignment(state, *r);

        updateAssembly(*r, *srec, &state);
    }

    auto rawAU = pack(id, &state);

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

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
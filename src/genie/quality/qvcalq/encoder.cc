/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/qvcalq/encoder.h"
#include <string>
#include <utility>
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/qvcalq/permutation.h"
#include "genie/util/watch.h"
#include "iostream"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvcalq {

using ClassType = core::record::ClassType;

paramqv1::Codebook codebookFromVector(const std::vector<unsigned char>& vec) {
    paramqv1::Codebook codebook(vec[0], vec[1]);
    for (size_t i = 2; i < vec.size(); ++i) {
        codebook.addEntry(vec[i]);
    }
    return codebook;
}

core::GenSubIndex get_qv_steps(size_t i) {
    UTILS_DIE_IF(i > 6, "QV_STEPS index out of range");
    return std::make_pair(core::GenDesc::QV, (uint16_t)i + 2);
}

void Encoder::setUpParametersAligned(const calq::DecodingBlock& block, paramqv1::QualityValues1& param) {
    // add codebooks from calq to param
    paramqv1::ParameterSet set;
    for (auto& codeVec : block.codeBooks) {
        auto codebook = codebookFromVector(codeVec);
        set.addCodeBook(std::move(codebook));
    }
    param.setQvps(std::move(set));
}

void Encoder::fillDescriptorAligned(calq::DecodingBlock& block, core::AccessUnit::Descriptor& desc) {
    // empty QV_PRESENT
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_PRESENT));

    // QV_CODEBOOK
    desc.add(core::AccessUnit::Subsequence(util::DataBlock(&block.quantizerIndices), core::GenSub::QV_CODEBOOK));

    // fill QV_STEPS_0-6
    for (size_t i = 0; i < block.stepindices.size(); ++i) {
        desc.add(core::AccessUnit::Subsequence(util::DataBlock(&block.stepindices[i]), get_qv_steps(i)));
    }
}

void Encoder::encodeAligned(const core::record::Chunk& chunk, paramqv1::QualityValues1& param,
                            core::AccessUnit::Descriptor& desc) {
    // objects required for calq
    calq::EncodingOptions encodingOptions;
    calq::SideInformation sideInformation;
    calq::EncodingBlock input;
    calq::DecodingBlock output;

    // fill calq objects
    for (auto& rec : chunk.getData()) {
        auto& f_segment = rec.getSegments().front();
        auto& f_alignment = rec.getAlignments().front();

        sideInformation.positions.push_back(f_alignment.getPosition());
        sideInformation.cigars.push_back(f_alignment.getAlignment().getECigar());
        sideInformation.sequences.push_back(f_segment.getSequence());
        input.qvalues.push_back(f_segment.getQualities().front());

        // add second read info
        if (rec.getSegments().size() == 2) {
            auto& s_segment = rec.getSegments()[1];

            UTILS_DIE_IF(rec.getAlignments().front().getAlignmentSplits().front()->getType() !=
                             core::record::AlignmentSplit::Type::SAME_REC,
                         "Wrong record type");

            auto& s_alignment = dynamic_cast<core::record::alignment_split::SameRec&>(
                *rec.getAlignments().front().getAlignmentSplits().front().get());

            sideInformation.positions.push_back(f_alignment.getPosition() + s_alignment.getDelta());
            sideInformation.cigars.push_back(s_alignment.getAlignment().getECigar());
            sideInformation.sequences.push_back(s_segment.getSequence());
            input.qvalues.push_back(s_segment.getQualities().front());
        }
    }

    // sort the values so the positions are in order
    auto p = sort_permutation(sideInformation.positions);
    sideInformation.positions = apply_permutation(sideInformation.positions, p);
    sideInformation.cigars = apply_permutation(sideInformation.cigars, p);
    sideInformation.sequences = apply_permutation(sideInformation.sequences, p);
    input.qvalues = apply_permutation(input.qvalues, p);

    // set offset
    sideInformation.posOffset = sideInformation.positions.front();
    sideInformation.qualOffset = 33;

    // encoding + filling genie objects
    calq::encode(encodingOptions, sideInformation, input, &output);
    setUpParametersAligned(output, param);
    fillDescriptorAligned(output, desc);

    return;
}

void Encoder::setUpUnaligned(paramqv1::QualityValues1& param, core::AccessUnit::Descriptor& desc) {
    // setup param
    paramqv1::ParameterSet set;
    auto codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
    set.addCodeBook(std::move(codebook));
    param.setQvps(std::move(set));

    // setup descriptor
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_PRESENT));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_CODEBOOK));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_0));
}

// from qvwriteout/encoder
void Encoder::addQualities(const core::record::Segment& s, core::AccessUnit::Descriptor& desc) {
    for (const auto& q : s.getQualities()) {
        for (const auto& c : q) {
            UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
            desc.get((uint16_t)desc.getSize() - 1).push(c - 33);
        }
    }
}

void Encoder::encodeUnaligned(const core::record::Chunk& chunk, paramqv1::QualityValues1& param,
                              core::AccessUnit::Descriptor& desc) {
    setUpUnaligned(param, desc);

    for (const auto& rec : chunk.getData()) {
        for (const auto& seg : rec.getSegments()) {
            addQualities(seg, desc);
        }
    }
}

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& chunk) {
    util::Watch watch;
    auto param = util::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
    core::AccessUnit::Descriptor desc(core::GenDesc::QV);

    const ClassType& classType = chunk.getData()[0].getClassID();
    calq::SideInformation sideInformation;
    calq::EncodingBlock encodingBlock;

    if (classType == ClassType::CLASS_U) {
        encodeUnaligned(chunk, *param, desc);
    } else {
        encodeAligned(chunk, *param, desc);
    }

    core::stats::PerfStats stats;
    stats.addDouble("time-qvcalq", watch.check());

    return std::make_tuple(std::move(param), std::move(desc), stats);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

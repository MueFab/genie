/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/qvcalq//encoder.h"
#include <string>
#include <utility>
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/util/watch.h"
#include "iostream"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvcalq {

using ClassType = core::record::ClassType;

template <typename T>
std::vector<std::size_t> sort_permutation(const std::vector<T>& vec) {
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&](std::size_t i, std::size_t j) { return vec[i] < vec[j]; });
    return p;
}

template <typename T>
std::vector<T> apply_permutation(const std::vector<T>& vec, const std::vector<std::size_t>& p) {
    std::vector<T> sorted_vec(vec.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(), [&](std::size_t i) { return vec[i]; });
    return sorted_vec;
}

// void Encoder::setUpParameters(const calq::DecodingBlock output, paramqv1::QualityValues1& param,
//                            core::AccessUnit::Descriptor& desc) {}


paramqv1::Codebook codebookFromVector(const std::vector<unsigned char>& vec){
    paramqv1::Codebook codebook(vec[0], vec[1]);
    for(size_t i=2; i<vec.size(); ++i){
        codebook.addEntry(vec[i]);
    }
    return codebook;
}

void Encoder::setUpParameters(const calq::DecodingBlock& block, paramqv1::QualityValues1& param, core::AccessUnit::Descriptor& desc) {

    // add codebooks from calq to param
    paramqv1::ParameterSet set;
    for(auto& codeVec:block.codeBooks){
        auto codebook = codebookFromVector(codeVec);
        set.addCodeBook(std::move(codebook));
    }
    param.setQvps(std::move(set));

    // setup desc
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_PRESENT));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_CODEBOOK));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_0));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_1));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_2));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_3));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_4));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_5));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_6));
}

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& chunk) {
    util::Watch watch;
    auto param = util::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
    core::AccessUnit::Descriptor desc(core::GenDesc::QV);

    const ClassType& classType = chunk.getData()[0].getClassID();
    calq::SideInformation sideInformation;
    calq::EncodingBlock encodingBlock;

    if (classType == ClassType::CLASS_U) {
        encodeAligned(chunk, desc);
        std::cout << "unmapped" << std::endl;
    } else {
        encodeAligned(chunk, desc);
    }

    UTILS_DIE("DEBUG exit");
    //    core::stats::PerfStats stats;
    //    stats.addDouble("time-qvcalq", watch.check());

    //    return std::make_tuple(std::move(param), std::move(desc), stats);
}

void Encoder::encodeAligned(const core::record::Chunk& chunk, core::AccessUnit::Descriptor& desc) {
    // default options
    calq::EncodingOptions encodingOptions;
    calq::SideInformation sideInformation;
    calq::EncodingBlock input;
    calq::DecodingBlock output;

    uint64_t size = 0;

    for (const auto& rec : chunk.getData()) {
        auto& f_segment = rec.getSegments().front();
        auto& f_alignment = rec.getAlignments().front();

        sideInformation.positions.push_back(f_alignment.getPosition());
        sideInformation.cigars.push_back(f_alignment.getAlignment().getECigar());
        sideInformation.sequences.push_back(f_segment.getSequence());
        input.qvalues.push_back(f_segment.getQualities().front());

        size += f_segment.getQualities().front().size();

        // add second read info
        if (rec.getSegments().size() == 2) {
            auto& s_segment = rec.getSegments()[1];

            UTILS_DIE_IF(rec.getAlignments().front().getAlignmentSplits().front()->getType() !=
                             core::record::AlignmentSplit::Type::SAME_REC,
                         "Wrong Type");

            auto& s_alignment = dynamic_cast<core::record::alignment_split::SameRec&>(
                *rec.getAlignments().front().getAlignmentSplits().front().get());

            sideInformation.positions.push_back(f_alignment.getPosition() + s_alignment.getDelta());
            sideInformation.cigars.push_back(s_alignment.getAlignment().getECigar());
            sideInformation.sequences.push_back(s_segment.getSequence());
            input.qvalues.push_back(s_segment.getQualities().front());

            size += s_segment.getQualities().front().size();
        }
    }

    // sort the values so the positions are in order
    auto p = sort_permutation(sideInformation.positions);
    sideInformation.positions = apply_permutation(sideInformation.positions, p);
    sideInformation.cigars = apply_permutation(sideInformation.cigars, p);
    sideInformation.sequences = apply_permutation(sideInformation.sequences, p);
    input.qvalues = apply_permutation(input.qvalues, p);

    sideInformation.posOffset = sideInformation.positions.front();

    calq::encode(encodingOptions, sideInformation, input, &output);

    // output.quantizerIndices.size() = 26076809
    //

    return;
}

void Encoder::encodeUnaligned(const core::record::Chunk& chunk, core::AccessUnit::Descriptor& desc) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

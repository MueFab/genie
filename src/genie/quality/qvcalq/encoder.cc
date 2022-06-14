/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/qvcalq//encoder.h"
#include <string>
#include <utility>
#include "calq/calq_coder.h"
#include "genie/core/record/alignment_split/same-rec.h"
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

template <typename T>
void apply_permutation_in_place(std::vector<T>& vec, const std::vector<std::size_t>& p) {
    std::vector<bool> done(vec.size());
    for (std::size_t i = 0; i < vec.size(); ++i) {
        if (done[i]) {
            continue;
        }
        done[i] = true;
        std::size_t prev_j = i;
        std::size_t j = p[i];
        while (i != j) {
            std::swap(vec[prev_j], vec[j]);
            done[j] = true;
            prev_j = j;
            j = p[j];
        }
    }
}

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& chunk) {
    //    util::Watch watch;

    const ClassType& classType = chunk.getData()[0].getClassID();
    calq::SideInformation sideInformation;
    calq::EncodingBlock encodingBlock;

    if (classType == ClassType::CLASS_U) {
        // TODO: Unmapped, maybe just copy qvwriteout
        std::cout << "unmapped" << std::endl;
    } else {
        encodeAligned(chunk);
    }

    UTILS_DIE("DEBUG exit");
    //    core::stats::PerfStats stats;
    //    stats.addDouble("time-qvcalq", watch.check());

    //    return std::make_tuple(std::move(param), std::move(desc), stats);
}

void Encoder::encodeAligned(const core::record::Chunk& chunk) {  // , core::AccessUnit::Descriptor& desc);
    // default options
    calq::EncodingOptions encodingOptions;
    calq::SideInformation sideInformation;
    calq::EncodingBlock input;
    calq::DecodingBlock output;

    for (const auto& rec : chunk.getData()) {
        auto& f_segment = rec.getSegments().front();
        auto& f_alignment = rec.getAlignments().front();

        sideInformation.positions.push_back(f_alignment.getPosition());
        sideInformation.cigars.push_back(f_alignment.getAlignment().getECigar());
        sideInformation.sequences.push_back(f_segment.getSequence());
        input.qvalues.push_back(f_segment.getQualities().front());

        // add second read info
        if (rec.getSegments().size() == 2) {
            auto& s_segment = rec.getSegments()[1];
            if (rec.getAlignments().front().getAlignmentSplits().front()->getType() !=
                core::record::AlignmentSplit::Type::SAME_REC) {
                UTILS_DIE("WRONG TYPE");
            }

            auto* s_alignment = dynamic_cast<const core::record::alignment_split::SameRec*>(
                rec.getAlignments().front().getAlignmentSplits().front().get());

            sideInformation.positions.push_back(f_alignment.getPosition() + s_alignment->getDelta());
            sideInformation.cigars.push_back(s_alignment->getAlignment().getECigar());
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

    // inplace sort
    //    apply_permutation_in_place(sideInformation.positions, p);
    //    apply_permutation_in_place(sideInformation.cigars, p);
    //    apply_permutation_in_place(sideInformation.sequences, p);
    //    apply_permutation_in_place(input.qvalues, p);

    sideInformation.posOffset = sideInformation.positions.front();

    calq::encode(encodingOptions, sideInformation, input, &output);

    return;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

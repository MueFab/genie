/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/encoder.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::calq {

using ClassType = core::record::ClassType;

// ---------------------------------------------------------------------------------------------------------------------

paramqv1::Codebook codebookFromVector(const std::vector<unsigned char>& vec) {
    paramqv1::Codebook codebook(vec[0], vec[1]);
    for (size_t i = 2; i < vec.size(); ++i) {
        codebook.addEntry(vec[i]);
    }
    return codebook;
}

// ---------------------------------------------------------------------------------------------------------------------

core::GenSubIndex get_qv_steps(size_t i) {
    UTILS_DIE_IF(i > 7, "QV_STEPS index out of range");
    return std::make_pair(core::GenDesc::QV, static_cast<uint16_t>(i + 2));
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::fillCalqStructures(const core::record::Chunk& chunk, calq::EncodingOptions& opt,
                                 calq::SideInformation& sideInformation, calq::EncodingBlock& input) {
    // fill calq objects
    for (auto& rec : chunk.getData()) {
        auto& f_segment = rec.getSegments().front();
        auto& f_alignment = rec.getAlignments().front();

        std::vector<std::string> qvalues;
        std::vector<std::string> sequences;
        std::vector<std::string> cigars;
        std::vector<uint64_t> positions;

        positions.push_back(f_alignment.getPosition());
        cigars.push_back(f_alignment.getAlignment().getECigar());
        sequences.push_back(f_segment.getSequence());
        qvalues.push_back(f_segment.getQualities().front());

        // add second read info
        if (rec.getSegments().size() == 2) {
            auto& s_segment = rec.getSegments()[1];

            UTILS_DIE_IF(rec.getAlignments().front().getAlignmentSplits().front()->getType() !=
                             core::record::AlignmentSplit::Type::SAME_REC,
                         "Wrong record type");

            sequences.push_back(s_segment.getSequence());
            qvalues.push_back(s_segment.getQualities().front());

            if (rec.getClassID() == core::record::ClassType::CLASS_HM) {
                positions.push_back(positions.back());
                // create cigar like "x+"
                std::string cigar = std::to_string(qvalues.back().size());
                cigar.append("+");
            } else {
                auto& s_alignment = dynamic_cast<core::record::alignment_split::SameRec&>(
                    *rec.getAlignments().front().getAlignmentSplits().front().get());

                positions.push_back(f_alignment.getPosition() + s_alignment.getDelta());
                cigars.push_back(s_alignment.getAlignment().getECigar());
            }
        }

        input.qvalues.emplace_back(std::move(qvalues));
        sideInformation.sequences.emplace_back(std::move(sequences));
        sideInformation.cigars.emplace_back(std::move(cigars));
        sideInformation.positions.emplace_back(std::move(positions));
    }

    // set offset and options
    sideInformation.posOffset = sideInformation.positions.front().front();
    opt.qualityValueOffset = 0;
    opt.qualityValueMin = 33;   // ascii !
    opt.qualityValueMax = 126;  // ascii ~

    //    auto classID = chunk.getData().front().getClassID();
    //    if(classID == genie::core::record::ClassType::CLASS_HM || classID == genie::core::record::ClassType::CLASS_U
    //    || classID == genie::core::record::ClassType::CLASS_I){
    //        opt.hasUnaligned = true;
    //    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeAligned(const core::record::Chunk& chunk, paramqv1::QualityValues1& param,
                            core::AccessUnit::Descriptor& desc) {
    // objects required for calq
    calq::EncodingOptions encodingOptions;
    calq::SideInformation sideInformation;
    calq::EncodingBlock input;
    calq::DecodingBlock output;

    fillCalqStructures(chunk, encodingOptions, sideInformation, input);

    calq::encode(encodingOptions, sideInformation, input, &output);

    // add codebooks from calq to param
    paramqv1::ParameterSet set;
    for (auto& codeVec : output.codeBooks) {
        auto codebook = codebookFromVector(codeVec);
        set.addCodeBook(std::move(codebook));
    }
    param.setQvps(std::move(set));

    // setup descriptor and fill
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_PRESENT));

    // QV_CODEBOOK
    desc.add(core::AccessUnit::Subsequence(util::DataBlock(&output.quantizerIndices), core::GenSub::QV_CODEBOOK));

    // fill QV_STEPS_0-7
    for (size_t i = 0; i < output.stepindices.size(); ++i) {
        desc.add(core::AccessUnit::Subsequence(util::DataBlock(&output.stepindices[i]), get_qv_steps(i)));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::addQualities(const core::record::Segment& s, core::AccessUnit::Descriptor& desc,
                           calq::UniformMinMaxQuantizer& quantizer) {
    auto& subsequence = desc.get((uint16_t)desc.getSize() - 1);

    for (const auto& q : s.getQualities()) {
        for (const auto& c : q) {
            auto index = static_cast<uint8_t>(quantizer.valueToIndex(c));
            subsequence.push(index);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeUnaligned(const core::record::Chunk& chunk, paramqv1::QualityValues1& param,
                              core::AccessUnit::Descriptor& desc) {
    // create quantizer
    calq::UniformMinMaxQuantizer quantizer(33, 126, 8);

    // set codebook
    std::vector<uint8_t> codebookVec;
    for (const auto& pair : quantizer.inverseLut()) {
        codebookVec.push_back(static_cast<uint8_t>(pair.second));
    }
    auto codebook = codebookFromVector(codebookVec);
    paramqv1::ParameterSet set;
    set.addCodeBook(std::move(codebook));
    param.setQvps(std::move(set));

    // setup descriptor
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_PRESENT));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_CODEBOOK));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_0));

    // encode values
    for (const auto& rec : chunk.getData()) {
        for (const auto& seg : rec.getSegments()) {
            addQualities(seg, desc, quantizer);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& chunk) {
    util::Watch watch;
    auto param = std::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
    core::AccessUnit::Descriptor desc(core::GenDesc::QV);

    const ClassType& classType = chunk.getData()[0].getClassID();

    if (classType == ClassType::CLASS_U) {
        encodeUnaligned(chunk, *param, desc);
    } else {
        encodeAligned(chunk, *param, desc);
    }

    core::stats::PerfStats stats;
    stats.addDouble("time-qvcalq", watch.check());

    return std::make_tuple(std::move(param), desc, stats);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::calq

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

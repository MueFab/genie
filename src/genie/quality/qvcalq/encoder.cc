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

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& chunk) {
    //    util::Watch watch;

    const ClassType& classType = chunk.getData()[0].getClassID();
    calq::SideInformation sideInformation;
    calq::EncodingBlock encodingBlock;

    if (classType == ClassType::CLASS_U) {
        // TODO: Unmapped, maybe just copy qvwriteout
        std::cout<<"unmapped"<<std::endl;
    } else {
        encodeAligned(chunk);
    }

    //    calq::EncodingOptions encodingOptions;  // TODO: option available over commandline
    //    calq::SideInformation sideInformation = getSideInformation(rec);
    //    calq::EncodingBlock input = getEncodingBlock(rec);
    //    calq::DecodingBlock output;
    //    calq::encode(encodingOptions, sideInformation, input, &output);

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
        if (rec.getSegments().size() == 2 ) {
//            std::cout<<"mapped 2  \n";
            auto& s_segment = rec.getSegments()[1];
            if(rec.getAlignments().front().getAlignmentSplits().front()->getType() != core::record::AlignmentSplit::Type::SAME_REC){
                auto type = rec.getAlignments().front().getAlignmentSplits().front()->getType();
                UTILS_DIE("WRONG TYPE");
            }

            auto* s_alignment = dynamic_cast<const core::record::alignment_split::SameRec*>(
                                    rec.getAlignments().front().getAlignmentSplits().front().get());

            auto pos = f_alignment.getPosition() + s_alignment->getDelta();
            if (pos == 25170530){
                std::cout<<"wrong"<<std::endl;
                continue ;
            }

            sideInformation.positions.push_back(f_alignment.getPosition() + s_alignment->getDelta());
            sideInformation.cigars.push_back(s_alignment->getAlignment().getECigar());
            sideInformation.sequences.push_back(s_segment.getSequence());

            input.qvalues.push_back(s_segment.getQualities().front());
        }
//        break;
    }

//    sideInformation.posOffset = sideInformation.positions.front();

    calq::encode(encodingOptions, sideInformation, input, &output);

    auto a = 10;
}

// calq::SideInformation Encoder::getSideInformation(const core::record::Chunk& chunk) {}
// calq::EncodingBlock Encoder::getEncodingBlock(const core::record::Chunk& record) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

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

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& rec) {
//    for (const auto& r : rec.getData()) {
//        auto seg = r.getSegments();
//    }

    calq::EncodingOptions encodingOptions; // TODO: option available over commandline
    calq::SideInformation sideInformation = getSideInformation(rec);
    calq::EncodingBlock input = getEncodingBlock(rec);
    calq::DecodingBlock output;
    calq::encode(encodingOptions, sideInformation, input, &output);
}

calq::SideInformation Encoder::getSideInformation(const core::record::Chunk& record) {}
calq::EncodingBlock Encoder::getEncodingBlock(const core::record::Chunk& record) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

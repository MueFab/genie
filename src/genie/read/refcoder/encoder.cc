/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/refcoder/encoder.h"
#include <memory>
#include <string>
#include <utility>
#include "genie/read/basecoder/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::refcoder {

// ---------------------------------------------------------------------------------------------------------------------

Encoder::RefEncodingState::RefEncodingState(const core::record::Chunk& data) : EncodingState(data) {
    excerpt = data.getRef();
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<std::string, std::string> Encoder::getReferences(const core::record::Record& r, EncodingState& state) {
    std::pair<std::string, std::string> ret;
    {
        auto begin = r.getAlignments().front().getPosition();
        auto length = core::record::Record::getLengthOfCigar(r.getAlignments().front().getAlignment().getECigar());
        auto end = begin + length;
        ret.first = dynamic_cast<RefEncodingState&>(state).excerpt.getString(begin, end);
    }

    if (r.getSegments().size() > 1) {
        const auto& srec = *reinterpret_cast<const core::record::alignment_split::SameRec*>(
            r.getAlignments().front().getAlignmentSplits().front().get());
        auto begin = r.getAlignments().front().getPosition() + srec.getDelta();
        auto length = core::record::Record::getLengthOfCigar(srec.getAlignment().getECigar());
        auto end = begin + length;
        ret.second = dynamic_cast<RefEncodingState&>(state).excerpt.getString(begin, end);
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Encoder::EncodingState> Encoder::createState(const core::record::Chunk& data) const {
    return std::make_unique<RefEncodingState>(data);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::refcoder

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

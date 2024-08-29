/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/refcoder/decoder.h"
#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/read/basecoder/decoder.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::refcoder {

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Decoder::getReferences(const basecoder::Decoder::SegmentMeta& meta, DecodingState& state) {
    std::vector<std::string> ret;
    auto& excerpt = dynamic_cast<RefDecodingState&>(state).refExcerpt;
    {
        auto begin = (uint32_t)meta.position[0];
        auto end = std::min<uint32_t>(begin + (uint32_t)meta.length[0], static_cast<uint32_t>(excerpt.getGlobalEnd()));
        auto clipsize = begin + (uint32_t)meta.length[0] - end;
        ret.emplace_back(excerpt.getString(begin, end) + std::string(clipsize, 'N'));
    }
    if (meta.num_segments == 2) {
        auto begin = (uint32_t)meta.position[1];
        auto end = std::min<uint32_t>(begin + (uint32_t)meta.length[1], static_cast<uint32_t>(excerpt.getGlobalEnd()));
        auto clipsize = begin + (uint32_t)meta.length[1] - end;
        ret.emplace_back(excerpt.getString(begin, end) + std::string(clipsize, 'N'));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<basecoder::DecoderStub::DecodingState> Decoder::createDecodingState(core::AccessUnit& t) {
    return util::make_unique<RefDecodingState>(t);
}

// ---------------------------------------------------------------------------------------------------------------------

Decoder::RefDecodingState::RefDecodingState(core::AccessUnit& t_data)
    : DecodingState(t_data), refExcerpt(t_data.getReferenceExcerpt()) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::refcoder

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

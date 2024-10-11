/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/localassembly/decoder.h"
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/global-cfg.h"
#include "genie/core/name-decoder.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/read/basecoder/decoder.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::localassembly {

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Decoder::getReferences(const basecoder::Decoder::SegmentMeta& meta, DecodingState& state) {
    std::vector<std::string> ret;
    ret.emplace_back(dynamic_cast<LADecodingState&>(state).refEncoder.getReference(
        static_cast<uint32_t>(meta.position[0]), static_cast<uint32_t>(meta.length[0])));
    if (meta.num_segments == 2) {
        ret.emplace_back(dynamic_cast<LADecodingState&>(state).refEncoder.getReference(
            static_cast<uint32_t>(meta.position[1]), static_cast<uint32_t>(meta.length[1])));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<basecoder::DecoderStub::DecodingState> Decoder::createDecodingState(core::AccessUnit& t) {
    return std::make_unique<LADecodingState>(t);
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::recordDecodedHook(DecodingState& state, const core::record::Record& rec) {
    dynamic_cast<LADecodingState&>(state).refEncoder.addRead(rec);
}

// ---------------------------------------------------------------------------------------------------------------------

Decoder::LADecodingState::LADecodingState(core::AccessUnit& t_data)
    : DecodingState(t_data), refEncoder(t_data.getParameters().getComputedRef().getExtension().getBufMaxSize()) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

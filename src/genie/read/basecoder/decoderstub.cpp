/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/basecoder/decoderstub.h"
#include <string>
#include <utility>
#include <vector>
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

// ---------------------------------------------------------------------------------------------------------------------

DecoderStub::DecodingState::DecodingState(core::AccessUnit& t_data) {
    numRecords = t_data.getNumReads();
    numSegments = t_data.getParameters().getNumberTemplateSegments();
    ref = t_data.getReference();
    qvStream = std::move(t_data.get(core::GenDesc::QV));
    nameStream = std::move(t_data.get(core::GenDesc::RNAME));
    qvparam = t_data.getParameters().getQVConfig(t_data.getClassType()).clone();
}

// ---------------------------------------------------------------------------------------------------------------------

core::record::Chunk DecoderStub::decodeSequences(DecodingState& state, core::AccessUnit& t_data) {
    util::Watch watch;
    core::record::Chunk chunk;
    chunk.setStats(std::move(t_data.getStats()));
    basecoder::Decoder decoder(std::move(t_data), state.numSegments, t_data.getMinPos());
    for (size_t recID = 0; recID < state.numRecords;) {
        auto meta = decoder.readSegmentMeta();

        auto refs = getReferences(meta, state);

        auto rec = decoder.pull(state.ref, std::move(refs), meta);
        addECigar(rec, state.ecigars);
        recordDecodedHook(state, rec);

        chunk.getData().emplace_back(std::move(rec));
        recID += meta.num_segments;
    }
    chunk.getStats().addDouble("time-basecoder", watch.check());
    return chunk;
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderStub::decodeNames(DecodingState& state, core::record::Chunk& chunk) {
    auto names = namecoder->process(state.nameStream);
    chunk.getStats().add(std::get<1>(names));
    for (size_t i = 0; i < std::get<0>(names).size(); ++i) {
        chunk.getData()[i].setName(std::get<0>(names)[i]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderStub::decodeQualities(DecodingState& state, core::record::Chunk& chunk) {
    auto qvs = qvcoder->process(*state.qvparam, state.ecigars, state.qvStream);
    chunk.getStats().add(std::get<1>(qvs));
    if (std::get<0>(qvs).empty()) {
        return;
    }
    size_t qvCounter = 0;
    for (auto& r : chunk.getData()) {
        auto& s_first = !r.isRead1First() && r.getSegments().size() == 2 ? r.getSegments()[1] : r.getSegments()[0];
        if (!std::get<0>(qvs)[qvCounter].empty()) {
            s_first.addQualities(std::move(std::get<0>(qvs)[qvCounter++]));
            r.setQVDepth(1);
        }

        if (r.getSegments().size() == 1) {
            continue;
        }

        auto& s_second = r.isRead1First() ? r.getSegments()[1] : r.getSegments()[0];
        if (!std::get<0>(qvs)[qvCounter].empty()) {
            s_second.addQualities(std::move(std::get<0>(qvs)[qvCounter++]));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderStub::addECigar(const core::record::Record& rec, std::vector<std::string>& cig_vec) {
    const auto& s_first =
        !rec.isRead1First() && rec.getSegments().size() == 2 ? rec.getSegments()[1] : rec.getSegments()[0];

    if (rec.getAlignments().empty()) {
        cig_vec.emplace_back(std::to_string(s_first.getSequence().length()) + '+');
    } else {
        cig_vec.emplace_back(rec.getAlignments().front().getAlignment().getECigar());
    }

    if (rec.getSegments().size() == 1) {
        return;
    }

    if (rec.getClassID() == core::record::ClassType::CLASS_HM) {
        const auto& s_second = rec.isRead1First() ? rec.getSegments()[1] : rec.getSegments()[0];
        cig_vec.emplace_back(std::to_string(s_second.getSequence().length()) + '+');
        return;
    }

    const auto& split = dynamic_cast<const core::record::alignment_split::SameRec*>(
        rec.getAlignments().front().getAlignmentSplits().front().get());
    cig_vec.emplace_back(split->getAlignment().getECigar());
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderStub::flowIn(core::AccessUnit&& t, const util::Section& id) {
    auto t_data = std::move(t);
    t_data = entropyCodeAU(std::move(t_data), true);
    auto state = createDecodingState(t_data);
    auto chunk = decodeSequences(*state, t_data);
    decodeQualities(*state, chunk);
    decodeNames(*state, chunk);
    flowOut(std::move(chunk), id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

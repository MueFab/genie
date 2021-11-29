/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/refcoder/decoder.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/read/basecoder/decoder.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace refcoder {

// ---------------------------------------------------------------------------------------------------------------------

Decoder::Decoder() : core::ReadDecoder() {}

// ---------------------------------------------------------------------------------------------------------------------

void addECigar(const core::record::Record& rec, std::vector<std::string>& cig_vec) {
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

void Decoder::flowIn(core::AccessUnit&& t, const util::Section& id) {
    util::Watch watch;
    auto t_data = std::move(t);
    auto minPos = t_data.getMinPos();
    auto ref_excerpt = t_data.getReferenceExcerpt();
    t_data = entropyCodeAU(std::move(t_data), true);
    size_t numRecords = t_data.getNumReads();
    size_t segments = t_data.getParameters().getNumberTemplateSegments();
    uint16_t ref = t_data.getReference();
    std::stringstream str;
    util::BitReader reader(str);
    core::record::Chunk chunk;
    const auto& qvparam = t_data.getParameters().getQVConfig(t_data.getClassType());
    auto qvStream = std::move(t_data.get(core::GenDesc::QV));
    t_data.getStats().addDouble("time-refcoder", watch.check());
    watch.reset();
    auto names = namecoder->process(t_data.get(core::GenDesc::RNAME));
    t_data.getStats().addDouble("time-name", watch.check());
    watch.reset();
    chunk.setStats(std::move(t_data.getStats()));
    basecoder::Decoder decoder(std::move(t_data), segments, minPos);
    std::vector<std::string> ecigars;
    size_t nameID = 0;
    for (size_t recID = 0; recID < numRecords;) {
        auto meta = decoder.readSegmentMeta();
        std::vector<std::string> refs;
        refs.reserve(meta.num_segments);
        recID += meta.num_segments;
        for (size_t i = 0; i < meta.num_segments; ++i) {
            auto end = std::min<uint32_t>((uint32_t)meta.position[i] + (uint32_t)meta.length[i],
                                          static_cast<uint32_t>(ref_excerpt.getGlobalEnd()));
            auto clipsize = (uint32_t)meta.position[i] + (uint32_t)meta.length[i] - end;
            refs.emplace_back(ref_excerpt.getString((uint32_t)meta.position[i], end) + std::string(clipsize, 'N'));
        }
        auto rec = decoder.pull(ref, std::move(refs), meta);
        if (!std::get<0>(names).empty()) {
            rec.setName(std::get<0>(names)[nameID++]);
        }
        addECigar(rec, ecigars);
        chunk.getData().emplace_back(std::move(rec));
    }
    chunk.getStats().addDouble("time-refcoder", watch.check());
    watch.reset();
    auto qvs = this->qvcoder->process(qvparam, ecigars, qvStream);
    size_t qvCounter = 0;
    if (!std::get<0>(qvs).empty()) {
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
    chunk.getStats().addDouble("time-qv", watch.check());
    decoder.clear();
    flowOut(std::move(chunk), id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

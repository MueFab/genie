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
    size_t num = 0;
    for (const auto& seg : rec.getSegments()) {
        if (rec.getAlignments().empty()) {
            cig_vec.emplace_back(std::to_string(seg.getSequence().length()) + '+');
            num++;
            continue;
        }
        if (!num) {
            cig_vec.emplace_back(rec.getAlignments().front().getAlignment().getECigar());
        } else {
            if (rec.getAlignments().front().getAlignmentSplits().size() <= num - 1) {
                auto* split = dynamic_cast<const core::record::alignment_split::SameRec*>(
                    rec.getAlignments().front().getAlignmentSplits()[num - 1].get());
                cig_vec.emplace_back(split->getAlignment().getECigar());
            } else {
                cig_vec.emplace_back(std::to_string(seg.getSequence().length()) + '+');
            }
        }
        num++;
    }
}

void Decoder::flowIn(core::AccessUnit&& t, const util::Section& id) {
    util::Watch watch;
    auto t_data = std::move(t);
    auto ref_excerpt = t_data.getReferenceExcerpt();
    t_data = entropyCodeAU(std::move(t_data), true);
    auto minPos = t_data.getMinPos();
    size_t numRecords = t_data.getNumReads();
    size_t segments = t_data.getParameters().getNumberTemplateSegments();
  //  uint16_t ref = t_data.getReference();
    std::stringstream str;
    util::BitReader reader(str);
    core::record::Chunk chunk;
    const auto& qvparam = t_data.getParameters().getQVConfig(t_data.getClassType());
    auto qvStream = std::move(t_data.get(core::GenDesc::QV));
    t_data.getStats().addDouble("time-localassembly", watch.check());
    watch.reset();
    auto names = namecoder->process(t_data.get(core::GenDesc::RNAME));
    t_data.getStats().addDouble("time-name", watch.check());
    watch.reset();
    chunk.setStats(std::move(t_data.getStats()));
    basecoder::Decoder decoder(std::move(t_data), segments, minPos);
    std::vector<std::string> ecigars;
    for (size_t recID = 0; recID < numRecords; ++recID) {
 /*       auto meta = decoder.readSegmentMeta();
        std::vector<std::string> refs;
        refs.reserve(meta.num_segments);
        for (const auto& m : meta) {
            refs.emplace_back(
                ref_excerpt.getString(m.position, std::min<size_t>(ref_excerpt.getGlobalEnd(), m.position + m.length)));
            refs.back().resize(m.length);
        }
        auto rec = decoder.pull(ref, std::move(refs));
        if (!std::get<0>(names).empty()) {
            rec.setName(std::get<0>(names)[recID]);
        }
        addECigar(rec, ecigars);
        chunk.getData().emplace_back(std::move(rec));*/
    }
    chunk.getStats().addDouble("time-localassembly", watch.check());
    watch.reset();
    auto qvs = this->qvcoder->process(qvparam, ecigars, qvStream);
    size_t qvCounter = 0;
    if (!std::get<0>(qvs).empty()) {
        for (auto& r : chunk.getData()) {
            for (auto& s : r.getSegments()) {
                if (!std::get<0>(qvs)[qvCounter].empty()) {
                    s.addQualities(std::move(std::get<0>(qvs)[qvCounter]));
                }
                qvCounter++;
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

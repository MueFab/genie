/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/lowlatency/decoder.h"
#include <string>
#include <utility>
#include <vector>
#include "genie/core/global_cfg.h"
#include "genie/core/name_decoder.h"
#include "genie/read/basecoder/decoder.h"
#include "genie/util/stop_watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace lowlatency {

// ---------------------------------------------------------------------------------------------------------------------

core::record::Chunk Decoder::decode_common(core::AccessUnit&& t) {
    util::Watch watch;
    core::record::Chunk ret;
    core::AccessUnit data = std::move(t);
    data = entropyCodeAU(std::move(data), true);
    const auto& qvparam = data.getParameters().getQVConfig(data.getClassType());
    auto qvStream = std::move(data.get(core::GenDesc::kQv));
    auto names = namecoder->process(data.get(core::GenDesc::kReadName));
    data.getStats().addDouble("time-name", watch.check());
    watch.reset();
    std::vector<std::string> ecigars;
    std::vector<uint64_t> positions;
    // FIXME: loop condition is only correct if all records have the full number of reads
    size_t i = 0;
    size_t rec_i = 0;
    while (i < data.getNumReads()) {
        core::record::Record rec(uint8_t(data.getParameters().getNumberTemplateSegments()),
                                 core::record::ClassType::CLASS_U,
                                 std::get<0>(names).empty() ? "" : std::move(std::get<0>(names)[rec_i]), "", 0);

        size_t num_segments = 1;
        if (data.getParameters().getNumberTemplateSegments() > 1) {
            auto decoding_case = data.pull(core::gen_sub::kPairDecodingCase);
            if (decoding_case == core::gen_const::kPairSameRecord) {
                num_segments = 2;
            } else if (decoding_case == core::gen_const::kPairR1Unpaired) {
                rec.SetRead1First(true);
            } else {
                rec.SetRead1First(false);
            }
        }

        for (size_t j = 0; j < num_segments; ++j) {
            size_t length = data.getParameters().getReadLength();
            ecigars.emplace_back(length, '+');
            positions.emplace_back(std::numeric_limits<uint64_t>::max());
            if (!length) {
                length = data.pull(core::gen_sub::kReadLength) + 1;
            }
            std::string seq(length, '\0');
            for (auto& c : seq) {
                c = core::GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[data.pull(core::gen_sub::kUnalignedReads)];
            }

            core::record::Segment seg(std::move(seq));
            //     seg.addQualities(qvdecoder->process(data.getParameters().getQVConfig(core::record::ClassType::CLASS_U),
            //                                        std::to_string(length) + "+", data.get(core::GenDesc::kQv)));
            rec.addSegment(std::move(seg));
        }

        ret.getData().push_back(std::move(rec));
        i += num_segments;
        rec_i++;
    }

    data.getStats().addDouble("time-lowlatency", watch.check());
    watch.reset();
    auto qvs = this->qvcoder->process(qvparam, ecigars, positions, qvStream);
    size_t qvCounter = 0;
    if (!std::get<0>(qvs).empty()) {
        for (auto& r : ret.getData()) {
            for (auto& s : r.getSegments()) {
                if (!std::get<0>(qvs)[qvCounter].empty()) {
                    s.addQualities(std::move(std::get<0>(qvs)[qvCounter]));
                }
                qvCounter++;
            }
            if (!r.getSegments().empty()) {
                r.setQVDepth(static_cast<uint8_t>(r.getSegments().front().getQualities().size()));
            }
        }
    }

    data.getStats().addDouble("time-qv", watch.check());
    watch.reset();

    ret.setStats(std::move(data.getStats()));
    data.clear();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::flowIn(core::AccessUnit&& t, const util::Section& id) { flowOut(decode_common(std::move(t)), id); }

// ---------------------------------------------------------------------------------------------------------------------

std::string Decoder::decode(core::AccessUnit&& t) {
    return decode_common(std::move(t)).getData().front().getSegments().front().getSequence();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lowlatency
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

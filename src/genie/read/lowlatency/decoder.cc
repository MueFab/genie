/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include <genie/core/global-cfg.h>
#include <genie/read/basecoder/decoder.h>
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace lowlatency {

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::flowIn(core::AccessUnitRaw&& t, size_t id) {
    core::record::Chunk ret;
    core::AccessUnitRaw data = std::move(t);

    std::stringstream str;
    util::BitReader reader(str);
    auto qvdecoder = core::GlobalCfg::getSingleton().getIndustrialPark().construct<core::QVDecoder>(
        data.getParameters().getQVConfig(core::record::ClassType::CLASS_U).getMode(), reader);

    for (size_t i = 0; i < data.getNumRecords() / data.getParameters().getNumberTemplateSegments(); ++i) {
        core::record::Record rec(data.getParameters().getNumberTemplateSegments(), core::record::ClassType::CLASS_U, "",
                                 "", 0);

        if (data.getParameters().getNumberTemplateSegments() > 1) {
            UTILS_DIE_IF(data.pull(core::GenSub::PAIR_DECODING_CASE) != core::GenConst::PAIR_SAME_RECORD,
                         "Only same record pairs supported");
            data.pull(core::GenSub::PAIR_SAME_REC);
        }

        for (size_t j = 0; j < data.getParameters().getNumberTemplateSegments(); ++j) {
            size_t length = data.getParameters().getReadLength();
            if (!length) {
                length = data.pull(core::GenSub::RLEN) + 1;
            }
            std::string seq(length, '\0');
            for (auto& c : seq) {
                c = core::getAlphabetProperties(core::AlphabetID::ACGTN).lut[data.pull(core::GenSub::UREADS)];
            }

            core::record::Segment seg(std::move(seq));
            seg.addQualities(qvdecoder->decode(data.getParameters().getQVConfig(core::record::ClassType::CLASS_U),
                                               std::to_string(length) + "+", data.get(core::GenDesc::QV)));
            rec.addSegment(std::move(seg));
        }

        ret.push_back(std::move(rec));
    }

    flowOut(std::move(ret), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lowlatency
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
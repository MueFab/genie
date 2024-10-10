/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/qvwriteout/encoder.h"
#include <memory>
#include <string>
#include <utility>
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::qvwriteout {

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::setUpParameters(const core::record::Chunk& rec, paramqv1::QualityValues1& param,
                              core::AccessUnit::Descriptor& desc) {
    paramqv1::ParameterSet set;

    auto codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
    set.addCodeBook(std::move(codebook));

    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_PRESENT));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_CODEBOOK));
    desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_0));
    if (rec.getData().front().getClassID() == core::record::ClassType::CLASS_I ||
        rec.getData().front().getClassID() == core::record::ClassType::CLASS_HM) {
        desc.add(core::AccessUnit::Subsequence(1, core::GenSub::QV_STEPS_1));

        codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
        set.addCodeBook(std::move(codebook));
    }

    param.setQvps(std::move(set));
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeAlignedSegment(const core::record::Segment& s, const std::string& ecigar,
                                   core::AccessUnit::Descriptor& desc) {
    for (const auto& q : s.getQualities()) {
        core::CigarTokenizer::tokenize(
            ecigar, core::getECigarInfo(),
            [&desc, &q](uint8_t cigar, const std::pair<size_t, size_t>& bs, const std::pair<size_t, size_t>&) -> bool {
                auto qvs = std::string_view(q).substr(bs.first, bs.second);
                uint8_t codebook = core::getECigarInfo().lut_step_ref[cigar] ||
                                           core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar)
                                       ? 2
                                       : (uint8_t)desc.getSize() - 1;
                for (const auto& c : qvs) {
                    UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
                    desc.get(codebook).push(c - 33);
                }
                return true;
            });
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeUnalignedSegment(const core::record::Segment& s, core::AccessUnit::Descriptor& desc) {
    for (const auto& q : s.getQualities()) {
        for (const auto& c : q) {
            UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
            desc.get((uint16_t)desc.getSize() - 1).push(c - 33);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& rec) {
    util::Watch watch;
    auto param = std::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
    core::AccessUnit::Descriptor desc(core::GenDesc::QV);

    setUpParameters(rec, *param, desc);

    for (const auto& r : rec.getData()) {
        auto& s_first = r.getSegments()[0];

        if (r.getAlignments().empty()) {
            encodeUnalignedSegment(s_first, desc);
        } else {
            encodeAlignedSegment(s_first, r.getAlignments().front().getAlignment().getECigar(), desc);
        }

        if (r.getSegments().size() == 1) {
            continue;
        }

        auto& s_second = r.getSegments()[1];

        if (r.getClassID() == core::record::ClassType::CLASS_HM || r.getClassID() == core::record::ClassType::CLASS_U) {
            encodeUnalignedSegment(s_second, desc);
        } else {
            encodeAlignedSegment(s_second,
                                 dynamic_cast<const core::record::alignment_split::SameRec*>(
                                     r.getAlignments().front().getAlignmentSplits().front().get())
                                     ->getAlignment()
                                     .getECigar(),
                                 desc);
        }
    }

    core::stats::PerfStats stats;
    stats.addDouble("time-qv1writeout", watch.check());
    return std::make_tuple(std::move(param), std::move(desc), stats);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::qvwriteout

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

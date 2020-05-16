/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include <genie/core/global-cfg.h>
#include <genie/core/name-decoder.h>
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/read/basecoder/decoder.h>
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

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

void Decoder::flowIn(core::AccessUnitRaw&& t, const util::Section& id) {
    size_t numRecords = t.getNumRecords();
    size_t bufSize = t.getParameters().getComputedRef().getExtension().getBufMaxSize();
    size_t segments = t.getParameters().getNumberTemplateSegments();
    uint16_t ref = t.getReference();
    LocalReference refEncoder(bufSize);
    std::stringstream str;
    util::BitReader reader(str);
    core::record::Chunk chunk;
    const auto& qvparam = t.getParameters().getQVConfig(t.getClassType());
    auto qvStream = std::move(t.get(core::GenDesc::QV));
    auto names = namecoder->process(t.get(core::GenDesc::RNAME));
    basecoder::Decoder decoder(std::move(t), segments);
    std::vector<std::string> ecigars;
    for (size_t recID = 0; recID < numRecords; ++recID) {
        auto meta = decoder.readSegmentMeta();
        std::vector<std::string> refs;
        refs.reserve(meta.size());
        for (const auto& m : meta) {
            refs.emplace_back(refEncoder.getReference(m.position, m.length));
        }
        auto rec = decoder.pull(ref, std::move(refs));
        if (!names.empty()) {
            rec.setName(names[recID]);
        }
        addECigar(rec, ecigars);
        refEncoder.addRead(rec);
        chunk.emplace_back(std::move(rec));
    }
    auto qvs = this->qvcoder->process(qvparam, ecigars, qvStream);
    size_t qvCounter = 0;
    for (auto& r : chunk) {
        for (auto& s : r.getSegments()) {
            if (!qvs[qvCounter].empty()) {
                s.addQualities(std::move(qvs[qvCounter]));
            }
            qvCounter++;
        }
    }

    flowOut(std::move(chunk), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/localassembly/encoder.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

// ---------------------------------------------------------------------------------------------------------------------

Encoder::LAEncodingState::LAEncodingState(const core::record::Chunk& data, uint32_t _cr_buf_max_size)
    : EncoderStub::EncodingState(data), refCoder(_cr_buf_max_size) {
    minPos = data.getData().front().getAlignments().front().getPosition();
    maxPos = 0;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::printDebug(const LAEncodingState& state, const std::string& ref1, const std::string& ref2,
                         const core::record::Record& r) const {
    if (!debug) {
        return;
    }
    state.refCoder.printWindow();
    std::cerr << "pair!" << std::endl;
    std::cerr << "ref1: " << std::endl;
    for (size_t i = 0; i < r.getAlignments().front().getPosition() - state.refCoder.getWindowBorder(); ++i) {
        std::cerr << " ";
    }
    std::cerr << ref1 << std::endl;

    std::cerr << "ref2: " << std::endl;
    for (size_t i = 0; i < r.getAlignments().front().getPosition() - state.refCoder.getWindowBorder(); ++i) {
        std::cerr << " ";
    }
    std::cerr << ref2 << std::endl;
    std::cerr << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit Encoder::pack(size_t id, core::QVEncoder::QVCoded qv, core::AccessUnit::Descriptor rname,
                               EncodingState& state) {
    auto ret = basecoder::EncoderStub::pack(id, std::move(qv), std::move(rname), state);

    auto crps = core::parameter::ComputedRef(core::parameter::ComputedRef::Algorithm::LOCAL_ASSEMBLY);
    crps.setExtension(
        core::parameter::ComputedRefExtended(0, dynamic_cast<LAEncodingState&>(state).refCoder.getMaxBufferSize()));
    ret.getParameters().setComputedRef(std::move(crps));

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<std::string, std::string> Encoder::getReferences(const core::record::Record& r, EncodingState& state) {
    std::pair<std::string, std::string> ret;
    {
        auto begin = r.getAlignments().front().getPosition();
        auto ecigar = r.getAlignments().front().getAlignment().getECigar();
        ret.first = dynamic_cast<LAEncodingState&>(state).refCoder.getReference(static_cast<uint32_t>(begin), ecigar);

        // Update AU end
        auto end = begin + core::record::Record::getLengthOfCigar(ecigar);
        state.maxPos = std::max(state.maxPos, end);
    }

    if (r.getSegments().size() > 1) {
        const auto& srec = *reinterpret_cast<const core::record::alignment_split::SameRec*>(
            r.getAlignments().front().getAlignmentSplits().front().get());
        auto begin = r.getAlignments().front().getPosition() + srec.getDelta();
        auto ecigar = srec.getAlignment().getECigar();
        ret.second = dynamic_cast<LAEncodingState&>(state).refCoder.getReference(static_cast<uint32_t>(begin), ecigar);

        // Update AU end
        auto end = begin + srec.getDelta() + core::record::Record::getLengthOfCigar(ecigar);
        state.maxPos = std::max(state.maxPos, end);
    }

    // Update computed reference
    dynamic_cast<LAEncodingState&>(state).refCoder.addRead(r);
    printDebug(dynamic_cast<LAEncodingState&>(state), ret.first, ret.second, r);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Encoder::EncodingState> Encoder::createState(const core::record::Chunk& data) const {
    const uint32_t READS_PER_ASSEMBLY = 10;
    uint32_t max_read_size = 0;
    for (const auto& r : data.getData()) {
        for (const auto& s : r.getSegments()) {
            max_read_size = std::max(max_read_size, static_cast<uint32_t>(s.getSequence().length()));
        }
    }
    return util::make_unique<LAEncodingState>(data, max_read_size * READS_PER_ASSEMBLY);
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(bool _debug) : debug(_debug) {}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    core::record::Chunk d = std::move(t);

    // Class N is not supported in local assembly becuase of the follwing situation:
    // AANAAAAAAAAA  <- Old reads
    //  ANAAAAAAAAAAA
    //
    //   AAAAAAANAAAAA  <- New read
    //   ^ Needs to patch N with A (not possible in class N), even though class of individual reads is correct
    // Downgrade to class M!
    for (auto& r : d.getData()) {
        if (r.getClassID() == core::record::ClassType::CLASS_N) {
            r.setClassType(core::record::ClassType::CLASS_M);
        }
    }

    basecoder::EncoderStub::flowIn(std::move(d), id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

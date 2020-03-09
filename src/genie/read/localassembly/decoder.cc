/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include <genie/core/global-cfg.h>
#include <genie/core/name-decoder.h>
#include <genie/read/basecoder/decoder.h>
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::flowIn(core::AccessUnitRaw&& t, size_t id) {
    size_t numRecords = t.getNumRecords();
    size_t bufSize = t.getParameters().getComputedRef().getExtension().getBufMaxSize();
    size_t segments = t.getParameters().getNumberTemplateSegments();
    uint16_t ref = t.getReference();
    LocalReference refEncoder(bufSize);
    std::stringstream str;
    util::BitReader reader(str);
    auto qvdecoder = core::GlobalCfg::getSingleton().getIndustrialPark().construct<core::QVDecoder>(
        t.getParameters().getQVConfig(core::record::ClassType::CLASS_U).getMode(), reader);
    auto namedecoder = core::GlobalCfg::getSingleton().getIndustrialPark().construct<core::NameDecoder>(0, reader);
    auto names = namedecoder->decode(t.get(core::GenDesc::RNAME));
    core::record::Chunk chunk;
    basecoder::Decoder decoder(std::move(t), segments);
    for (size_t recID = 0; recID < numRecords; ++recID) {
        auto meta = decoder.readSegmentMeta();
        std::vector<std::string> refs;
        refs.reserve(meta.size());
        for (const auto& m : meta) {
            refs.emplace_back(refEncoder.getReference(m.position, m.length));
        }
        auto rec = decoder.pull(ref, *qvdecoder, std::move(names[recID]), std::move(refs));
        refEncoder.addRead(rec);
        chunk.emplace_back(std::move(rec));
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
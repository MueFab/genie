/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-decompressor.h"
#include <sstream>
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

core::AccessUnitRaw::Subsequence GabacDecompressor::decompress(const gabac::EncodingConfiguration& conf,
                                                               core::AccessUnitPayload::SubsequencePayload&& data,
                                                               util::DataBlock* const dependency) {
    core::AccessUnitPayload::SubsequencePayload in = std::move(data);
    // Interface to GABAC library
    util::DataBlock buffer = in.move();
    gabac::IBufferStream in_stream(&buffer, 0);

    gabac::IBufferStream *bufferDependencyStream = nullptr;
    if(dependency != nullptr) {
        bufferDependencyStream = new gabac::IBufferStream(dependency);
    }

    util::DataBlock tmp(0, 4);
    gabac::OBufferStream outbuffer(&tmp);

    // Setup
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream* const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&in_stream, bufferDependencyStream, &outbuffer, GABAC_BLOCK_SIZE, GABC_LOG_OUTPUT_STREAM,
                                                   gabac::IOConfiguration::LogLevel::TRACE};
    const bool GABAC_DECODING_MODE = true;

    // Run
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);

    if(bufferDependencyStream != nullptr) {
        delete bufferDependencyStream;
    }

    outbuffer.flush(&tmp);
    return core::AccessUnitRaw::Subsequence(std::move(tmp), in.getID());
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::flowIn(core::AccessUnitPayload&& t, size_t id) {
    core::AccessUnitPayload payloadSet = std::move(t);
    GabacSeqConfSet configSet;
    auto raw_aus = core::AccessUnitRaw(payloadSet.moveParameters(), payloadSet.getRecordNum());

    configSet.loadParameters(raw_aus.getParameters());

    for (auto& desc : payloadSet) {
        if (desc.isEmpty()) {
            continue;
        }
        for (auto& subseq : desc) {
            if (subseq.isEmpty()) {
                continue;
            }
            auto descSubseqID = subseq.getID();
            core::AccessUnitRaw::Subsequence subseqData =
                decompress(configSet.getConfAsGabac(descSubseqID),
                           std::move(subseq),
                           raw_aus.getSubsequenceDependency(descSubseqID));
            raw_aus.set(descSubseqID, std::move(subseqData));
        }
    }

    raw_aus.setReference(payloadSet.getReference());
    payloadSet.clear();
    flowOut(std::move(raw_aus), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::dryIn() { dryOut(); }
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

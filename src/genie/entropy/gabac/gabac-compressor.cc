/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-compressor.h"
#include <genie/util/make-unique.h>

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

void writeVInt(uint64_t val, util::DataBlock &out) {
    std::vector<uint8_t> tmp;
    do {
        tmp.push_back(val & 0x7f);
        val >>= 7;
    } while (val != 0);
    for (int i = tmp.size() - 1; i > 0; i--) out.push_back(tmp[i] | 0x80);
    out.push_back(tmp[0]);
}

static util::DataBlock convertTokenType(const util::DataBlock &in) {
    util::DataBlock out(0, 1);
    uint32_t num_output_symbols = in.empty() ? 0 : 256 * (256 * (256 * in.get(0) + in.get(1)) + in.get(2)) + in.get(3);
    writeVInt(num_output_symbols, out);
    if (!in.empty()) {
        for (size_t i = 4; i < in.size(); ++i) {
            out.push_back(in.get(i));
        }
    }
    return out;
}

core::AccessUnitPayload::SubsequencePayload GabacCompressor::compress(const gabac::EncodingConfiguration &conf,
                                                                      core::AccessUnitRaw::Subsequence &&in) {
    // Interface to GABAC library
    core::AccessUnitRaw::Subsequence data = std::move(in);
    size_t num_symbols = data.getNumSymbols();
    util::DataBlock buffer = data.move();
    gabac::IBufferStream bufferInputStream(&buffer);

    util::DataBlock outblock(0, 1);
    gabac::OBufferStream bufferOutputStream(&outblock);

    // Setup
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream *const GABAC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, nullptr, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                   GABAC_LOG_OUTPUT_STREAM, gabac::IOConfiguration::LogLevel::TRACE};
    const bool GABAC_DECODING_MODE = false;

    // Run
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);

    bufferOutputStream.flush(&outblock);
    core::AccessUnitPayload::SubsequencePayload out(data.getID());
    out.annotateNumSymbols(num_symbols);
    out.set(std::move(outblock));
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacCompressor::flowIn(core::AccessUnitRaw &&t, const util::Section& id) {
    core::AccessUnitRaw raw_aus = std::move(t);
    auto payload = core::AccessUnitPayload(raw_aus.moveParameters(), raw_aus.getNumRecords());

    for (const auto &desc : raw_aus) {
        core::AccessUnitPayload::DescriptorPayload descriptor_payload(desc.getID());
        for (const auto &subdesc : desc) {
            auto &input = raw_aus.get(subdesc.getID());
            if(input.getNumSymbols() > 0) {
                const auto &conf = configSet.getConfAsGabac(subdesc.getID());
                // add compressed payload
                descriptor_payload.add(compress(conf, std::move(input)));
            } else {
                // add empty payload
                descriptor_payload.add(core::AccessUnitPayload::SubsequencePayload(subdesc.getID(), util::DataBlock(0,1)));
            }
        }
        if (!descriptor_payload.isEmpty()) {
            payload.setPayload(desc.getID(), std::move(descriptor_payload));
        }
    }

    configSet.storeParameters(payload.getParameters());

    payload.setReference(raw_aus.getReference());
    payload.setMinPos(raw_aus.getMinPos());
    payload.setMaxPos(raw_aus.getMaxPos());
    raw_aus.clear();
    flowOut(std::move(payload), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacCompressor::dryIn() { dryOut(); }

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

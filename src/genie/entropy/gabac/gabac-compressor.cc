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

util::DataBlock convertTokenType(const util::DataBlock &in) {
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
                                                                      bool vSize,
                                                                      core::AccessUnitRaw::Subsequence &&in) {
    // Interface to GABAC library
    core::AccessUnitRaw::Subsequence data = std::move(in);
    util::DataBlock buffer = data.move();
    gabac::IBufferStream bufferInputStream(&buffer);

    GenieGabacOutputStream bufferOutputStream;

    // Setup
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream *const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                   GABC_LOG_OUTPUT_STREAM, gabac::IOConfiguration::LogLevel::TRACE};
    const bool GABAC_DECODING_MODE = false;

    // Run
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);

    // Translate to GENIE-Payload TODO: change gabac to directly output the right format
    std::vector<util::DataBlock> outbuffer;
    bufferOutputStream.flush_blocks(&outbuffer);
    size_t index = 0;
    core::AccessUnitPayload::SubsequencePayload out(data.getID());
    for (auto &o : outbuffer) {
        if(vSize) {
            o = convertTokenType(o);
        }
        out.add(core::AccessUnitPayload::TransformedPayload(std::move(o), index++));
    }
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacCompressor::flowIn(core::AccessUnitRaw &&t, size_t id) {
    core::AccessUnitRaw raw_aus = std::move(t);
    auto payload = core::AccessUnitPayload(raw_aus.moveParameters(), raw_aus.getNumRecords());

    for (const auto &desc : raw_aus) {
        core::AccessUnitPayload::DescriptorPayload descriptor_payload(desc.getID());
        for (const auto &subdesc : desc) {
            auto &input = raw_aus.get(subdesc.getID());
            auto s_id = subdesc.getID();
            if (desc.getID() == core::GenDesc::RNAME || desc.getID() == core::GenDesc::MSAR) {
                s_id.second = 0;
            }
            const auto &conf = configSet.getConfAsGabac(s_id);
            descriptor_payload.add(compress(
                conf, desc.getID() == core::GenDesc::RNAME || desc.getID() == core::GenDesc::MSAR, std::move(input)));
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

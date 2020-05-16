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

core::AccessUnitPayload::DescriptorPayload GabacCompressor::compressTokens(const gabac::EncodingConfiguration &conf0,
                                                                           core::AccessUnitRaw::Descriptor &&in) {
    auto desc = std::move(in);
    util::DataBlock block(0, 1);
    gabac::OBufferStream stream(&block);
    util::BitWriter writer(&stream);
    size_t num_streams = 0;
    for (const auto &subsequence : desc) {
        if (subsequence.getNumSymbols()) {
            num_streams++;
        }
    }

    if(num_streams == 0) {
        return core::AccessUnitPayload::DescriptorPayload(desc.getID());
    }

    writer.write(desc.begin()->getNumSymbols(), 32);
    writer.write(num_streams, 16);

    for (auto &subsequence : desc) {
        if (subsequence.getNumSymbols()) {
            writer.write(subsequence.getID().second & 0xfu, 4);
            writer.write(3, 4);
            compress(conf0, std::move(subsequence)).write(writer);
        }
    }

    stream.flush(&block);
    core::AccessUnitPayload::DescriptorPayload ret(desc.getID());
    ret.add(core::AccessUnitPayload::SubsequencePayload({desc.getID(),0}, std::move(block)));
    return ret;
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
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream,      nullptr,
                                                   &bufferOutputStream,     GABAC_BLOCK_SIZE,
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

void GabacCompressor::flowIn(core::AccessUnitRaw &&t, const util::Section &id) {
    core::AccessUnitRaw raw_aus = std::move(t);
    auto payload = core::AccessUnitPayload(raw_aus.moveParameters(), raw_aus.getNumRecords());

    for (auto &desc : raw_aus) {
        core::AccessUnitPayload::DescriptorPayload descriptor_payload(desc.getID());
        auto ID = desc.getID();
        if (!getDescriptor(desc.getID()).tokentype) {
            for (const auto &subdesc : desc) {
                auto &input = raw_aus.get(subdesc.getID());
                if (input.getNumSymbols() > 0) {
                    const auto &conf = configSet.getConfAsGabac(subdesc.getID());
                    // add compressed payload
                    descriptor_payload.add(compress(conf, std::move(input)));
                } else {
                    // add empty payload
                    descriptor_payload.add(
                        core::AccessUnitPayload::SubsequencePayload(subdesc.getID(), util::DataBlock(0, 1)));
                }
            }
        } else {
            const auto &conf = configSet.getConfAsGabac({desc.getID(), 0});
            descriptor_payload = compressTokens(conf, std::move(desc));
        }
        if (!descriptor_payload.isEmpty()) {
            payload.setPayload(ID, std::move(descriptor_payload));
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

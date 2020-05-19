/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-compressor.h"
#include <genie/util/make-unique.h>
#include <genie/util/watch.h>

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

    if (num_streams == 0) {
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
    ret.add(core::AccessUnitPayload::SubsequencePayload({desc.getID(), 0}, std::move(block)));
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
    util::Watch watch;
    core::AccessUnitRaw raw_aus = std::move(t);
    auto payload = core::AccessUnitPayload(raw_aus.moveParameters(), raw_aus.getNumRecords());
    payload.setStats(std::move(raw_aus.getStats()));

    size_t totalSizeCompressed = 0;
    size_t totalSizeUncompressed = 0;
    for (auto &desc : raw_aus) {
        core::AccessUnitPayload::DescriptorPayload descriptor_payload(desc.getID());
        auto ID = desc.getID();
        if (!getDescriptor(desc.getID()).tokentype) {
            for (const auto &subdesc : desc) {
                totalSizeUncompressed += raw_aus.get(subdesc.getID()).getNumSymbols() * sizeof(uint32_t);
                if (raw_aus.get(subdesc.getID()).getNumSymbols()) {
                    payload.getStats().addInteger(
                        "size-" + getDescriptor(desc.getID()).name + std::to_string(subdesc.getID().second) + "-raw",
                        raw_aus.get(subdesc.getID()).getNumSymbols() * sizeof(uint32_t));
                }
                auto &input = raw_aus.get(subdesc.getID());
                if (input.getNumSymbols() > 0) {
                    const auto &conf = configSet.getConfAsGabac(subdesc.getID());
                    // add compressed payload
                    auto compressed = compress(conf, std::move(input));
                    if (compressed.getNumSymbols()) {
                        payload.getStats().addInteger("size-" + getDescriptor(desc.getID()).name +
                                                          std::to_string(subdesc.getID().second) + "-comp",
                                                      compressed.get().getRawSize());
                    }
                    totalSizeCompressed += compressed.get().getRawSize();
                    descriptor_payload.add(std::move(compressed));
                } else {
                    // add empty payload
                    descriptor_payload.add(
                        core::AccessUnitPayload::SubsequencePayload(subdesc.getID(), util::DataBlock(0, 1)));
                }
            }
        } else {
            size_t size = 0;
            for(const auto& s : desc) {
                size += s.getNumSymbols() * sizeof(uint32_t);
            }
            std::string name = getDescriptor(desc.getID()).name;
            if(size) {
                payload.getStats().addInteger("size-" + name + "-raw", size);
            }
            const auto &conf = configSet.getConfAsGabac({desc.getID(), 0});
            descriptor_payload = compressTokens(conf, std::move(desc));
            if(size) {
                payload.getStats().addInteger("size-" + name + "-comp", descriptor_payload.begin()->get().getRawSize());
            }
        }
        if (!descriptor_payload.isEmpty()) {
            payload.setPayload(ID, std::move(descriptor_payload));
        }
    }

    payload.getStats().addInteger("size-total-comp", totalSizeCompressed);
    payload.getStats().addInteger("size-total-raw", totalSizeUncompressed);

    configSet.storeParameters(payload.getParameters());

    payload.setReference(raw_aus.getReference());
    payload.setMinPos(raw_aus.getMinPos());
    payload.setMaxPos(raw_aus.getMaxPos());
    payload.setClassType(raw_aus.getClassType());
    raw_aus.clear();
    payload.getStats().addDouble("time-gabac", watch.check());
    flowOut(std::move(payload), id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

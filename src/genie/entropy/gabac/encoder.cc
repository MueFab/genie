/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/encoder.h"
#include <iostream>
#include <string>
#include <utility>
#include "genie/util/make-unique.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Descriptor Encoder::compressTokens(const gabac::EncodingConfiguration &conf0,
                                                     core::AccessUnit::Descriptor &&in) {
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
        auto ret_empty = core::AccessUnit::Descriptor(desc.getID());
        // add empty payload
        ret_empty.add(core::AccessUnit::Subsequence({desc.getID(), (uint16_t)0}, util::DataBlock(0, 1)));
        return ret_empty;
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
    core::AccessUnit::Descriptor ret(desc.getID());
    ret.add(core::AccessUnit::Subsequence({desc.getID(), (uint8_t)0}, std::move(block)));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence Encoder::compress(const gabac::EncodingConfiguration &conf,
                                                core::AccessUnit::Subsequence &&in) {
    // Interface to GABAC library
    core::AccessUnit::Subsequence data = std::move(in);
    size_t num_symbols = data.getNumSymbols();
    util::DataBlock buffer = data.move();
    gabac::IBufferStream bufferInputStream(&buffer);

    gabac::IBufferStream *bufferDependencyStream = nullptr;
    if (data.getDependency() != nullptr) {
        bufferDependencyStream = new gabac::IBufferStream(data.getDependency());
    }

    util::DataBlock outblock(0, 1);
    gabac::OBufferStream bufferOutputStream(&outblock);

    // Setup
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream *const GABAC_LOG_OUTPUT_STREAM = &std::cerr;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream,      bufferDependencyStream,
                                                   &bufferOutputStream,     GABAC_BLOCK_SIZE,
                                                   GABAC_LOG_OUTPUT_STREAM, gabac::IOConfiguration::LogLevel::LOG_TRACE};
    const bool GABAC_DECODING_MODE = false;

    // Run
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);

    bufferOutputStream.flush(&outblock);
    core::AccessUnit::Subsequence out(data.getID());
    out.annotateNumSymbols(num_symbols);
    out.set(std::move(outblock));

    if (bufferDependencyStream != nullptr) {
        delete bufferDependencyStream;
    }

    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

core::EntropyEncoder::EntropyCoded Encoder::process(core::AccessUnit::Descriptor &desc) {
    EntropyCoded ret;
    util::Watch watch;
    std::get<1>(ret) = std::move(desc);
    if (!getDescriptor(std::get<1>(ret).getID()).tokentype) {
        for (auto &subdesc : std::get<1>(ret)) {
            if (!subdesc.isEmpty()) {
                const auto &conf = configSet.getConfAsGabac(subdesc.getID());
                // add compressed payload
                auto id = subdesc.getID();

                std::get<2>(ret).addInteger("size-gabac-total-raw", subdesc.getRawSize());
                std::get<2>(ret).addInteger("size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-" +
                                                core::getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name +
                                                "-raw",
                                            subdesc.getRawSize());

                std::get<1>(ret).set(id.second, compress(conf, std::move(subdesc)));

                if (!std::get<1>(ret).get(id.second).isEmpty()) {
                    std::get<2>(ret).addInteger("size-gabac-total-comp", std::get<1>(ret).get(id.second).getRawSize());
                    std::get<2>(ret).addInteger(
                        "size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-" +
                            core::getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name + "-comp",
                        std::get<1>(ret).get(id.second).getRawSize());
                }
            } else {
                // add empty payload
                std::get<1>(ret).set(subdesc.getID().second,
                                     core::AccessUnit::Subsequence(subdesc.getID(), util::DataBlock(0, 1)));
            }
        }
        configSet.storeParameters(std::get<1>(ret).getID(), std::get<0>(ret));
    } else {
        size_t size = 0;
        for (const auto &s : std::get<1>(ret)) {
            size += s.getNumSymbols() * sizeof(uint32_t);
        }
        std::string name = getDescriptor(std::get<1>(ret).getID()).name;
        const auto &conf = configSet.getConfAsGabac({std::get<1>(ret).getID(), (uint16_t)0});
        std::get<1>(ret) = compressTokens(conf, std::move(std::get<1>(ret)));
        configSet.storeParameters(std::get<1>(ret).getID(), std::get<0>(ret));

        if (size) {
            std::get<2>(ret).addInteger("size-gabac-total-raw", size);
            std::get<2>(ret).addInteger("size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-raw",
                                        size);
            std::get<2>(ret).addInteger("size-gabac-total-comp", std::get<1>(ret).begin()->getRawSize());
            std::get<2>(ret).addInteger("size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-comp",
                                        std::get<1>(ret).begin()->getRawSize());
        }
    }
    std::get<2>(ret).addDouble("time-gabac", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

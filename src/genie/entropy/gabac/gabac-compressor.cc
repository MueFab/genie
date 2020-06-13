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

core::AccessUnit::Descriptor GabacCompressor::compressTokens(const gabac::EncodingConfiguration &conf0,
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
        return core::AccessUnit::Descriptor(desc.getID());
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
    ret.add(core::AccessUnit::Subsequence({desc.getID(), 0}, std::move(block)));
    return ret;
}

core::AccessUnit::Subsequence GabacCompressor::compress(const gabac::EncodingConfiguration &conf,
                                                                      core::AccessUnit::Subsequence &&in) {
    // Interface to GABAC library
    core::AccessUnit::Subsequence data = std::move(in);
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
    core::AccessUnit::Subsequence out(data.getID());
    out.annotateNumSymbols(num_symbols);
    out.set(std::move(outblock));
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

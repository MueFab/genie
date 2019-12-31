/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-compressor.h"
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/descriptor_configuration_present.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::SubsequencePayload GabacCompressor::compress(const gabac::EncodingConfiguration &conf,
                                                              MpeggRawAu::SubDescriptor&& in) {
    // Interface to GABAC library
    MpeggRawAu::SubDescriptor data = std::move(in);
    gabac::DataBlock buffer = data.move();
    gabac::IBufferStream bufferInputStream(&buffer);

    lae::GenieGabacOutputStream bufferOutputStream;

    // Setup
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream *const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                   GABC_LOG_OUTPUT_STREAM, gabac::IOConfiguration::LogLevel::TRACE};
    const bool GABAC_DECODING_MODE = false;

    // Run
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);

    // Translate to GENIE-Payload TODO: change gabac to directly output the right format
    std::vector<gabac::DataBlock> outbuffer;
    bufferOutputStream.flush_blocks(&outbuffer);
    size_t index = 0;
    BlockPayloadSet::SubsequencePayload out(data.getID());
    for (auto &o : outbuffer) {
        out.add(BlockPayloadSet::TransformedPayload(std::move(o), index++));
    }
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacCompressor::flowIn(MpeggRawAu&& t, size_t id) {
    MpeggRawAu&& raw_aus = std::move(t);
    auto payload = BlockPayloadSet(raw_aus.moveParameters(), raw_aus.getNumRecords());

    for (const auto &desc : raw_aus.getDescriptorStreams()) {
        BlockPayloadSet::DescriptorPayload descriptor_payload(desc.getID());
        for (const auto &subdesc : desc.getSubsequences()) {
            auto &input = raw_aus.get(subdesc.getID());
            const auto &conf = configSet.getConfAsGabac(subdesc.getID());
            descriptor_payload.add(compress(conf, std::move(input)));
        }
        if (!descriptor_payload.isEmpty()) {
            payload.setPayload(desc.getID(), std::move(descriptor_payload));
        }
    }

    configSet.storeParameters(payload.getParameters());

    raw_aus.clear();
    flowOut(std::move(payload), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacCompressor::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

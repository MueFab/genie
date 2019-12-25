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

void GabacCompressor::compress(const gabac::EncodingConfiguration &conf, MpeggRawAu::SubDescriptor *in,
                               BlockPayloadSet::SubsequencePayload *out) {
    // Interface to GABAC library
    gabac::DataBlock buffer(0, 4);
    in->swap(&buffer);
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
    for (auto &o : outbuffer) {
        out->add(util::make_unique<BlockPayloadSet::TransformedPayload>(&o));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacCompressor::flowIn(std::unique_ptr<MpeggRawAu> raw_aus, size_t id) {
    auto payload = util::make_unique<BlockPayloadSet>(raw_aus->moveParameters(), raw_aus->getNumRecords());

    for (const auto &desc : raw_aus->getDescriptorStreams()) {
        auto descriptor_payload = util::make_unique<BlockPayloadSet::DescriptorPayload>(desc->getID());
        for (const auto &subdesc : desc->getSubsequences()) {
            auto &input = raw_aus->get(desc->getID(), subdesc->getID());
            const auto &conf = configSet.getConfAsGabac(desc->getID(), subdesc->getID());
            auto subsequence_payload = util::make_unique<BlockPayloadSet::SubsequencePayload>(subdesc->getID());
            compress(conf, &input, subsequence_payload.get());

            descriptor_payload->add(std::move(subsequence_payload));
        }
        if (!descriptor_payload->isEmpty()) {
            payload->setPayload(desc->getID(), std::move(descriptor_payload));
        }
    }

    configSet.storeParameters(payload->getParameters());

    flowOut(std::move(payload), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacCompressor::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

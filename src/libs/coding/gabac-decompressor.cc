#include "gabac-decompressor.h"

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::decompress(const gabac::EncodingConfiguration& conf,
                                   const BlockPayloadSet::SubsequencePayload& in, MpeggRawAu::SubDescriptor* out) {
    // Interface to GABAC library
    std::stringstream in_stream;
    for (const auto& payload : in.getTransformedPayloads()) {
        gabac::DataBlock buffer(0, 1);
        payload->swap(&buffer);

        uint32_t size = buffer.getRawSize();
        in_stream.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
        in_stream.write(reinterpret_cast<char*>(buffer.getData()), buffer.getRawSize());
    }

    gabac::DataBlock tmp(0, 4);
    gabac::OBufferStream outbuffer(&tmp);

    // Setup
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream* const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&in_stream, &outbuffer, GABAC_BLOCK_SIZE, GABC_LOG_OUTPUT_STREAM,
                                                   gabac::IOConfiguration::LogLevel::TRACE};
    const bool GABAC_DECODING_MODE = true;

    // Run
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);

    outbuffer.flush(&tmp);
    out->swap(&tmp);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::flowIn(std::unique_ptr<BlockPayloadSet> payloadSet, size_t id) {
    GabacSeqConfSet configSet;
    auto raw_aus = util::make_unique<MpeggRawAu>(payloadSet->moveParameters(), payloadSet->getRecordNum());

    configSet.loadParameters(*raw_aus->getParameters());

    for (const auto& desc : payloadSet->getPayloads()) {
        if (!desc) {
            continue;
        }
        for (const auto& subseq : desc->getSubsequencePayloads()) {
            if (!subseq) {
                continue;
            }
            auto subseqData = util::make_unique<MpeggRawAu::SubDescriptor>(4, subseq->getID());
            decompress(configSet.getConfAsGabac(desc->getID(), subseq->getID()), *subseq, subseqData.get());
            raw_aus->set(desc->getID(), subseq->getID(), std::move(subseqData));
        }
    }

    flowOut(std::move(raw_aus), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
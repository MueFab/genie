#include "gabac-decompressor.h"

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor GabacDecompressor::decompress(const gabac::EncodingConfiguration& conf,
                                   BlockPayloadSet::SubsequencePayload&& data) {
    BlockPayloadSet::SubsequencePayload in = std::move(data);
    // Interface to GABAC library
    std::stringstream in_stream;
    for (auto& payload : in.getTransformedPayloads()) {
        gabac::DataBlock buffer = payload.move();

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
    return MpeggRawAu::SubDescriptor(std::move(tmp), in.getID());
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::flowIn(BlockPayloadSet&& t, size_t id) {
    BlockPayloadSet payloadSet = std::move(t);
    GabacSeqConfSet configSet;
    auto raw_aus = MpeggRawAu(payloadSet.moveParameters(), payloadSet.getRecordNum());

    configSet.loadParameters(raw_aus.getParameters());

    for (auto& desc : payloadSet.getPayloads()) {
        if (desc.isEmpty()) {
            continue;
        }
        for (auto& subseq : desc.getSubsequencePayloads()) {
            if (subseq.isEmpty()) {
                continue;
            }
            auto d_id = subseq.getID();
            MpeggRawAu::SubDescriptor subseqData = decompress(configSet.getConfAsGabac(subseq.getID()), std::move(subseq));
            raw_aus.set(d_id, std::move(subseqData));
        }
    }

    raw_aus.setReference(payloadSet.getReference());
    payloadSet.clear();
    flowOut(std::move(raw_aus), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
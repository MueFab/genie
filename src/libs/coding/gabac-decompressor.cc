#include "gabac-decompressor.h"

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::decompress(const gabac::EncodingConfiguration& conf, std::vector<gabac::DataBlock>* in,
                                   gabac::DataBlock* out) {
    // Interface to GABAC library
    std::stringstream in_stream;
    for (auto i : *in) {
        uint32_t size = i.getRawSize();
        in_stream.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
        in_stream.write(reinterpret_cast<char*>(i.getData()), i.getRawSize());
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

    outbuffer.flush(out);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::flowIn(std::unique_ptr<BlockPayloadSet> t, size_t id) {
    GabacSeqConfSet configSet;
    auto raw_aus = util::make_unique<MpeggRawAu>(t->moveParameters());

    configSet.loadParameters(*raw_aus->getParameters());

    for (auto const& desc : getDescriptors()) {
        if (!t->getPayload(desc.id)) {
            continue;
        }
        for (auto const& sub_desc : desc.subseqs) {
            std::vector<gabac::DataBlock> input;
            gabac::DataBlock output(0, 4);
            for (const auto& trans : t->getPayload(desc.id)->getSubDescriptorPayload(sub_desc.id)->get()) {
                input.emplace_back(0, 1);
                input.back().swap(trans->get());
            }
            decompress(configSet.getConfAsGabac(desc.id, sub_desc.id), &input, &output);
            auto tmp = util::make_unique<MpeggRawAu::SubDescriptor>(&output);
            raw_aus->set(desc.id, sub_desc.id, std::move(tmp));
        }
    }

    flowOut(std::move(raw_aus), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
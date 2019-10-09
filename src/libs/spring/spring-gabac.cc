// Copyright 2018 The genie authors

#include "spring-gabac.h"
#include <format/part2/parameter_set.h>
#include <format/part2/access_unit.h>
#include <format/part2/clutter.h>

namespace spring {

GenieGabacOutputBuffer::GenieGabacOutputBuffer() : bytesLeft(0) {}

int GenieGabacOutputBuffer::overflow(int c) {
    char c2 = c;
    xsputn(&c2, sizeof(char));
    return c;
}

std::streamsize GenieGabacOutputBuffer::xsputn(const char *s, std::streamsize n) {
    std::streamsize curPos = 0;
    while (curPos < n) {
        // Fill size buffer
        while (bytesLeft == 0 && sizeBuf.size() != sizeof(uint32_t) && curPos < n) {
            sizeBuf.push_back(s[curPos++]);
        }

        // Size buffer full, create new stream
        if (sizeBuf.size() == sizeof(uint32_t)) {
            bytesLeft = *reinterpret_cast<uint32_t *>(sizeBuf.data());
            sizeBuf.clear();
            streams.emplace_back(bytesLeft, sizeof(uint8_t));
        }

        // Copy data if possible
        size_t l = std::min(n - curPos, bytesLeft);
        memcpy(static_cast<uint8_t *>(streams.back().getData()) + (streams.back().size() - bytesLeft), s + curPos, l);
        curPos += l;
        bytesLeft -= l;
    }
    return n;
}

void GenieGabacOutputBuffer::flush_blocks(std::vector<gabac::DataBlock> *dat) {
    if (bytesLeft) {
        GABAC_DIE("Stream not in flushable state, bytes left");
    }
    if (!sizeBuf.empty()) {
        GABAC_DIE("Stream not in flushable state, expecting size");
    }
    dat->clear();
    dat->swap(streams);
}

void gabac_compress(const gabac::EncodingConfiguration& conf, gabac::DataBlock* in, std::vector<gabac::DataBlock>* out) {

    // Interface to GABAC library
    gabac::IBufferStream bufferInputStream(in);
    GenieGabacOutputStream bufferOutputStream;

    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream *const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                   GABC_LOG_OUTPUT_STREAM,
                                                   gabac::IOConfiguration::LogLevel::TRACE};

    const bool GABAC_DECODING_MODE = false;
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);
    bufferOutputStream.flush_blocks(out);
}

std::vector<std::vector<std::vector<gabac::DataBlock>>> create_default_streams() {
    std::vector<std::vector<std::vector<gabac::DataBlock>>> ret(format::NUM_DESCRIPTORS);
    for(int descriptor = 0; descriptor < format::NUM_DESCRIPTORS; ++descriptor) {
      if (descriptor == 15)
        ret[descriptor].resize(128*6);
      else
        ret[descriptor].resize(format::getDescriptorProperties()[descriptor].number_subsequences);
    }
    return ret;
}

std::vector<std::vector<gabac::EncodingConfiguration>> create_default_conf() {
    const std::vector<size_t> SEQUENCE_NUMS = {2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 3, 2, 1, 1};
    const std::string DEFAULT_GABAC_CONF_JSON =
            "{"
            "\"word_size\": 4,"
            "\"sequence_transformation_id\": 0,"
            "\"sequence_transformation_parameter\": 0,"
            "\"transformed_sequences\":"
            "[{"
            "\"lut_transformation_enabled\": false,"
            "\"diff_coding_enabled\": false,"
            "\"binarization_id\": 0,"
            "\"binarization_parameters\":[32],"
            "\"context_selection_id\": 0"
            "}]"
            "}";
    const std::string DEFAULT_GABAC_CONF_TOKENTYPE_JSON =
            "{"
            "\"word_size\": 1,"
            "\"sequence_transformation_id\": 0,"
            "\"sequence_transformation_parameter\": 0,"
            "\"transformed_sequences\":"
            "[{"
            "\"lut_transformation_enabled\": false,"
            "\"diff_coding_enabled\": false,"
            "\"binarization_id\": 0,"
            "\"binarization_parameters\":[8],"
            "\"context_selection_id\": 0"
            "}]"
            "}";
    std::vector<std::vector<gabac::EncodingConfiguration>> ret;
    for (size_t i = 0; i < SEQUENCE_NUMS.size(); ++i) {
        ret.emplace_back();
        for (size_t j = 0; j < SEQUENCE_NUMS[i]; ++j) {
            if (i == 11 || i == 15) // msar or rname
              ret[i].emplace_back(DEFAULT_GABAC_CONF_TOKENTYPE_JSON);
            else
              ret[i].emplace_back(DEFAULT_GABAC_CONF_JSON);
        }
    }
    return ret;
}

}  // namespace spring

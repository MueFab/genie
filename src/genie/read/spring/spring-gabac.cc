/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "spring-gabac.h"
#include <genie/core/parameter/parameter_set.h>
#include <genie/format/mgb/access_unit.h>
#include <fstream>

namespace genie {
namespace read {
namespace spring {

/*GenieGabacOutputBuffer::GenieGabacOutputBuffer() : bytesLeft(0) {}

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

void GenieGabacOutputBuffer::flush_blocks(std::vector<util::DataBlock> *dat) {
    if (bytesLeft) {
        GABAC_DIE("Stream not in flushable state, bytes left");
    }
    if (!sizeBuf.empty()) {
        GABAC_DIE("Stream not in flushable state, expecting size");
    }
    dat->clear();
    dat->swap(streams);
}

void gabac_compress(const gabac::EncodingConfiguration &conf, util::DataBlock *in,
                    std::vector<util::DataBlock> *out) {
    // Interface to GABAC library
    gabac::IBufferStream bufferInputStream(in);
    GenieGabacOutputStream bufferOutputStream;

    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream *const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                   GABC_LOG_OUTPUT_STREAM, gabac::IOConfiguration::LogLevel::TRACE};

    const bool GABAC_DECODING_MODE = false;
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);
    bufferOutputStream.flush_blocks(out);
}

std::vector<std::vector<util::DataBlock>> generate_empty_raw_data() {
    std::vector<std::vector<util::DataBlock>> raw_data(mgb::NUM_DESCRIPTORS);
    for (size_t descriptor = 0; descriptor < mgb::NUM_DESCRIPTORS; ++descriptor) {
        if (descriptor == 15) {
            raw_data[descriptor].resize(128 * 6);
            for (int i = 0; i < 128; i++) {
                for (int j = 0; j < 6; j++) {
                    raw_data[descriptor][6 * i + j].setWordSize(1);
                }
            }
        } else {
            raw_data[descriptor].resize(mgb::getDescriptorProperties()[descriptor].number_subsequences);
            for (int subseq = 0; subseq < mgb::getDescriptorProperties()[descriptor].number_subsequences;
subseq++) { raw_data[descriptor][subseq].setWordSize(4);
            }
        }
    }
    return raw_data;
}

std::vector<std::vector<std::vector<util::DataBlock>>> create_default_streams() {
    std::vector<std::vector<std::vector<util::DataBlock>>> ret(mgb::NUM_DESCRIPTORS);
    for (size_t descriptor = 0; descriptor < mgb::NUM_DESCRIPTORS; ++descriptor) {
        if (descriptor == 15)
            ret[descriptor].resize(128 * 6);
        else
            ret[descriptor].resize(mgb::getDescriptorProperties()[descriptor].number_subsequences);
    }
    return ret;
}

uint64_t write_streams_to_file(const std::vector<std::vector<std::vector<util::DataBlock>>> &generated_streams,
                           const std::string &outfile, const std::vector<uint8_t> &descriptors_to_write) {
    uint64_t retval = 0;
    std::ofstream fout(outfile, std::ios::binary);
    for (auto & descriptor : descriptors_to_write) {
        for (size_t subseq = 0; subseq < generated_streams[descriptor].size(); subseq++) {
            // write number of gabac data blocks
            uint32_t ndb = (uint32_t)generated_streams[descriptor][subseq].size();
            fout.write((char *)&ndb, sizeof(uint32_t));
            retval += sizeof(uint32_t);
            // now write each data block
            for (uint32_t db = 0; db < generated_streams[descriptor][subseq].size(); db++) {
                uint8_t wordSize = generated_streams[descriptor][subseq][db].getWordSize();
                size_t size = generated_streams[descriptor][subseq][db].size();
                size_t len_data_in_bytes = size * wordSize;
                // first write word size
                fout.write((char *)&wordSize, sizeof(uint8_t));
                retval += sizeof(uint8_t);
                // size
                fout.write((char *)&size, sizeof(size_t));
                retval += sizeof(size_t);
                // data
                fout.write((char *)generated_streams[descriptor][subseq][db].getData(), len_data_in_bytes);
                retval += len_data_in_bytes;
            }
        }
    }
    fout.close();
    return retval;
}

// assumes that the generated_streams vector is already initialized with
// create_default_streams.
uint64_t read_streams_from_file(std::vector<std::vector<std::vector<util::DataBlock>>> &generated_streams, const
std::string &infile, const std::vector<uint8_t> &descriptors_to_read) { uint64_t retval = 0; std::ifstream fin(infile,
std::ios::binary); for (auto &descriptor : descriptors_to_read) { for (size_t subseq = 0; subseq <
generated_streams[descriptor].size(); subseq++) {
            // read number of gabac data blocks
            uint32_t num_data_blocks;
            fin.read((char *)&(num_data_blocks), sizeof(uint32_t));
            retval += sizeof(uint32_t);
            generated_streams[descriptor][subseq].resize(num_data_blocks);
            // now read each data block
            for (uint32_t db = 0; db < generated_streams[descriptor][subseq].size(); db++) {
                uint8_t wordSize;
                fin.read((char *)&wordSize, sizeof(uint8_t));
                retval += sizeof(uint8_t);
                size_t size;
                fin.read((char *)&size, sizeof(size_t));
                retval += sizeof(size_t);
                size_t len_data_in_bytes = size * wordSize;
                generated_streams[descriptor][subseq][db].resize(size);
                fin.read((char *)generated_streams[descriptor][subseq][db].getData(), len_data_in_bytes);
                retval += len_data_in_bytes;
            }
        }
    }
    fin.close();
    return retval;
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
            if (i == 11 || i == 15)  // msar or rname
                ret[i].emplace_back(DEFAULT_GABAC_CONF_TOKENTYPE_JSON);
            else
                ret[i].emplace_back(DEFAULT_GABAC_CONF_JSON);
        }
    }
    return ret;
}*/

}
}  // namespace read
}  // namespace genie
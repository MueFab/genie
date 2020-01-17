/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SPRING_GABAC_H
#define GENIE_SPRING_GABAC_H

#include <genie/entropy/gabac/gabac.h>

namespace genie {
namespace read {
namespace spring {

// to use for writing compressed files to temporary files and reload them in combine-aus
const std::vector<uint8_t> read_descriptors = {0, 1, 3, 4, 6, 7, 8, 12};
const std::vector<uint8_t> id_descriptors = {15};
const std::vector<uint8_t> quality_descriptors = {14};

class GenieGabacOutputBuffer : public std::streambuf {
   public:
    GenieGabacOutputBuffer();

   protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char *s, std::streamsize n) override;

   private:
    std::vector<util::DataBlock> streams;
    std::streamsize bytesLeft;
    std::vector<uint8_t> sizeBuf;

   public:
    void flush_blocks(std::vector<util::DataBlock> *dat);
};

class GenieGabacOutputStream : public GenieGabacOutputBuffer, public std::ostream {
   public:
    GenieGabacOutputStream() : GenieGabacOutputBuffer(), std::ostream(this) {}
};

void gabac_compress(const entropy::gabac::EncodingConfiguration &conf, util::DataBlock *in,
                    std::vector<util::DataBlock> *out);

std::vector<std::vector<std::vector<util::DataBlock>>> create_default_streams();

std::vector<std::vector<util::DataBlock>> generate_empty_raw_data();

uint64_t write_streams_to_file(const std::vector<std::vector<std::vector<util::DataBlock>>> &generated_streams,
                               const std::string &outfile, const std::vector<uint8_t> &descriptors_to_write);

uint64_t read_streams_from_file(std::vector<std::vector<std::vector<util::DataBlock>>> &generated_streams,
                                const std::string &infile, const std::vector<uint8_t> &descriptors_to_read);

std::vector<std::vector<entropy::gabac::EncodingConfiguration>> create_default_conf();

}  // namespace spring
}  // namespace read
}  // namespace genie

#endif  // GENIE_GENIE_GABAC_OUTPUT_STREAM_H

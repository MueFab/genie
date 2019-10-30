// Copyright 2018 The genie authors

#ifndef GENIE_SPRING_GABAC_H
#define GENIE_SPRING_GABAC_H

#include <gabac/gabac.h>

namespace spring {

// to use for writing compressed files to temporary files and reload them in combine-aus
const std::vector<uint8_t> read_descriptors = {0,1,3,4,6,7,8,12};
const std::vector<uint8_t> id_descriptors = {15};
const std::vector<uint8_t> quality_descriptors = {14};

class GenieGabacOutputBuffer : public std::streambuf {
   public:
    GenieGabacOutputBuffer();

   protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char *s, std::streamsize n) override;

   private:
    std::vector<gabac::DataBlock> streams;
    std::streamsize bytesLeft;
    std::vector<uint8_t> sizeBuf;

   public:
    void flush_blocks(std::vector<gabac::DataBlock> *dat);
};

class GenieGabacOutputStream : public GenieGabacOutputBuffer, public std::ostream {
   public:
    GenieGabacOutputStream() : GenieGabacOutputBuffer(), std::ostream(this) {}
};

void gabac_compress(const gabac::EncodingConfiguration &conf, gabac::DataBlock *in, std::vector<gabac::DataBlock> *out);

std::vector<std::vector<std::vector<gabac::DataBlock>>> create_default_streams();

std::vector<std::vector<gabac::DataBlock>> generate_empty_raw_data();

void write_streams_to_file(const std::vector<std::vector<std::vector<gabac::DataBlock>>> &generated_streams,
                           const std::string &outfile, const std::vector<uint8_t> &descriptors_to_write);

void read_streams_from_file(std::vector<std::vector<std::vector<gabac::DataBlock>>> &generated_streams, const std::string &infile, const std::vector<uint8_t> &descriptors_to_read);

std::vector<std::vector<gabac::EncodingConfiguration>> create_default_conf();

}  // namespace spring

#endif  // GENIE_GENIE_GABAC_OUTPUT_STREAM_H

// Copyright 2018 The genie authors

#ifndef GENIE_SPRING_GABAC_H
#define GENIE_SPRING_GABAC_H

#include <gabac/gabac.h>

namespace spring {

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
                           const std::string &outfile);

void read_streams_from_file(std::vector<std::vector<std::vector<gabac::DataBlock>>> &generated_streams, const std::string &infile, const std::vector<uint8_t> &descriptors_to_read);

std::vector<std::vector<gabac::EncodingConfiguration>> create_default_conf();

}  // namespace spring

#endif  // GENIE_GENIE_GABAC_OUTPUT_STREAM_H

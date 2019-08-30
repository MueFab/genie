#ifndef GENIE_STREAM_SAVER_H
#define GENIE_STREAM_SAVER_H

#include <filesystem/filesystem.h>
#include <stdint.h>
#include <atomic>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>
#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif
#include <ureads-encoder/logger.h>

#include <gabac/gabac.h>

namespace dsg {
struct AcessUnitStreams {
    std::map<uint8_t, std::map<uint8_t, gabac::DataBlock>> streams;
};
class StreamSaver {
   private:
    void run_gabac(const std::string &stream_name, gabac::DataBlock *data, bool decompression);

    struct Streampos {
        std::streampos position;
        uint64_t size;
    };

   public:
    std::ostream *fout;  // Output file
    std::istream *fin;   // Output file

    std::map<std::string, gabac::EncodingConfiguration> configs;  // Already loaded configurations

    std::map<std::string, Streampos> file_index;

    std::string configPath;  // Path to gabac config directory

    void analyze(const std::string &name, gabac::DataBlock *data);
    void compress(const std::string &name, gabac::DataBlock *data);
    void decompress(const std::string &name, gabac::DataBlock *data);

    void pack(const gabac::DataBlock &data, const std::string &stream_name);  // Pack stream into file
    void unpack(const std::string &stream_name, gabac::DataBlock *data);      // Pack stream into file

    struct gabac_stream_params {
        uint64_t maxval;   // Maximum value in stream
        uint8_t wordsize;  // Maximum working word size
    };

    static const std::map<std::string, gabac_stream_params> &getParams();  // Get reference to hardcoded map.

    static const std::string &getDefaultConf();

    void buildIndex();

    void finish();

   public:
    static std::string getConfigName(const std::string &stream);

    void loadConfig(const std::string &name);

    void reloadConfigSet();

    StreamSaver(const std::string &configp, std::ostream *f, std::istream *ifile);

    ~StreamSaver();
};
}  // namespace dsg
#endif  // GENIE_STREAM_SAVER_H

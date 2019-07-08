#ifndef GENIE_STREAMSAVER_H
#define GENIE_STREAMSAVER_H

#include <atomic>
#include <vector>
#include <stdint.h>
#include <string>
#include <set>
#include <iostream>
#include <regex>
#include <map>
#include <filesystem/filesystem.hpp>
#include <omp.h>
#include <ureads-encoder/logger.h>

#include "thread_pool.h"
#include "gabac/streams.h"
#include "gabac/configuration.h"
#include "gabac/run.h"
#include "gabac/analysis.h"

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

        std::ostream *fout; // Output file
        std::istream *fin; // Output file

        std::map<std::string, gabac::EncodingConfiguration> configs; // Already loaded configurations

        std::map<std::string, Streampos> file_index;

        std::string configPath; // Path to gabac config directory

        void compress(const std::string &name, gabac::DataBlock *data);
        void decompress(const std::string &name, gabac::DataBlock *data);

        void pack(const gabac::DataBlock &data, const std::string &stream_name); // Pack stream into file
        void unpack(const std::string &stream_name, gabac::DataBlock *data); // Pack stream into file

        struct gabac_stream_params {
            uint64_t maxval; // Maximum value in stream
            uint8_t wordsize; // Maximum working word size
        };

        static const std::map<std::string, gabac_stream_params> &getParams(); // Get reference to hardcoded map.

        static const std::string &getDefaultConf();

        void buildIndex();

    public:
        static std::string getConfigName(const std::string &stream);

        void loadConfig(const std::string &name);


        StreamSaver(const std::string &configp, std::ostream *f, std::istream *ifile);

        ~StreamSaver();

    };
}
#endif //GENIE_STREAMSAVER_H
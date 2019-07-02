#ifndef GENIE_STREAMSTOREMAN_H
#define GENIE_STREAMSTOREMAN_H

#include <vector>
#include <stdint.h>
#include <string>
#include <set>
#include <iostream>
#include <map>
#include <filesystem/filesystem.hpp>

#include "thread_pool.h"
#include "gabac/streams.h"
#include "gabac/configuration.h"
#include "gabac/run.h"
#include "gabac/analysis.h"

namespace dsg {
    class StreamCompressor {
    public:
        virtual void compress(const std::string &stream, std::vector<int64_t> *subseq, gabac::DataBlock *out) = 0;
        virtual ~StreamCompressor() = default;
    };

    class StreamPacker {
    public:
        virtual void pack(const gabac::DataBlock &data, const std::string &stream_name) = 0;
        virtual ~StreamPacker() = default;
    };

    class StreamCompressorGabac : public StreamCompressor{
    private:

        std::map<std::string, gabac::EncodingConfiguration> configs;
        std::set<std::string> active_configs;
        std::mutex stateLock;
        std::condition_variable wait_var;

        std::string configPath;

        std::string getConfigName(const std::string &stream);

        gabac::EncodingConfiguration loadConfig(const gabac::DataBlock &data, const std::string &configName);

    public:
        void compress(const std::string &stream, std::vector<int64_t> *subseq, gabac::DataBlock *out) override;

        StreamCompressorGabac(const std::string& p);
    };

    class StreamPackerGenie : public StreamPacker {
    private:
        FILE *fout;
        std::mutex fileLock;

    public:
        void pack(const gabac::DataBlock &data, const std::string &stream_name) override;
        StreamPackerGenie(FILE* f);
    };

    class StreamStoreman {
    private:
        ThreadPool pool;
        StreamCompressor* compressor;
        StreamPacker* packer;

        uint32_t writeCounter;
        uint32_t totalCounter;

        std::mutex stateLock;
        std::condition_variable wait_var;

        bool deterministic;

    public:
        StreamStoreman(uint8_t numThread, StreamPacker* p, StreamCompressor* c, bool det);

        void store(std::vector<int64_t> *subseq, const std::string &stream_name);

        void wait();
    };
}
#endif //GENIE_STREAMSTOREMAN_H

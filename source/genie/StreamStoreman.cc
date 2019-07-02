#include "StreamStoreman.h"

namespace dsg {
    std::string StreamCompressorGabac::getConfigName(const std::string &stream) {
        return "Test";
    }

    gabac::EncodingConfiguration StreamCompressorGabac::loadConfig(const gabac::DataBlock &data,
                                                                   const std::string &configName) {
        {
            std::unique_lock<std::mutex> guard(stateLock);

            // Config is cached
            auto it = configs.find(configName);
            if (it != configs.end()) {
                return it->second;
            }

            // Config is not cached - mark it as being processed
            if (active_configs.find(configName) != active_configs.end()) {
                // Config is already being processed, wait for running process.
                wait_var.wait(guard, [&]() {
                    return active_configs.find(configName) == active_configs.end();
                });

                return configs.at(configName);
            }

            // Load config
            active_configs.insert(configName);
        }

        // Config not on hard drive, regenerate
        if (!ghc::filesystem::exists(ghc::filesystem::path(configPath + configName + ".json"))) {
            std::ofstream outstream(configPath + configName + ".json", std::ios::binary);
            gabac::DataBlock d = data;
            gabac::IBufferStream instream(&d);
            gabac::IOConfiguration
                    ioconf{&instream, &outstream, std::min(uint64_t(d.getRawSize()), uint64_t(10000000u)),
                           &std::cout, gabac::IOConfiguration::LogLevel::WARNING};
            gabac::analyze(ioconf, gabac::getCandidateConfig());
        }

        // Load config from file
        std::ifstream t(configPath + configName + ".json");
        if (!t) {
            throw std::runtime_error("Config not existent");
        }
        std::string configstring = std::string((std::istreambuf_iterator<char>(t)),
                                               std::istreambuf_iterator<char>());

        gabac::EncodingConfiguration enconf(configstring);

        // Save config to cache and notify other threads
        {
            std::unique_lock<std::mutex> guard(stateLock);

            configs[configName] = enconf;
            active_configs.erase(configName);
        }

        wait_var.notify_all();

        return enconf;
    }

    void StreamCompressorGabac::compress(const std::string &stream,
                                         std::vector<int64_t> *subseq,
                                         gabac::DataBlock *out) {
        gabac::DataBlock inBlock(subseq);
        gabac::EncodingConfiguration enConf(loadConfig(inBlock, getConfigName(stream)));
        uint64_t insize = inBlock.getRawSize();
        gabac::IBufferStream istream(&inBlock, 0);
        gabac::OBufferStream ostream(out);

        // Configure gabac streams
        gabac::IOConfiguration
                ioconf = {&istream, &ostream, std::min(uint64_t(insize), uint64_t(1000000000u)), &std::cout,
                          gabac::IOConfiguration::LogLevel::TRACE};

        gabac::run(ioconf, enConf, false);
    }

    StreamCompressorGabac::StreamCompressorGabac(const std::string &p) : configPath(p) {

    }


    void StreamPackerGenie::pack(const gabac::DataBlock &data, const std::string &stream_name) {
        std::lock_guard<std::mutex> guard(fileLock);
        // Write name of input file
        uint64_t size = stream_name.size();
        if (fwrite(&size, sizeof(uint64_t), 1, fout) != 1) {
            throw std::runtime_error("Could not write to output file");
        }
        if (fwrite(stream_name.c_str(), 1, size, fout) != size) {
            throw std::runtime_error("Could not write to output file");
        }

        // Get and write size of input file
        size = data.getRawSize();
        if (fwrite(&size, sizeof(uint64_t), 1, fout) != 1) {
            throw std::runtime_error("Could not write to output file");
        }

        if (fwrite(data.getData(), data.getRawSize(), 1, fout) != 1) {
            throw std::runtime_error("Could not write to output file");
        }
    }

    StreamPackerGenie::StreamPackerGenie(FILE *f) : fout(f) {
    }

    StreamStoreman::StreamStoreman(uint8_t numThread, StreamPacker *p, StreamCompressor *c, bool det) : pool(numThread),
                                                                                              compressor(c),
                                                                                              packer(p),
                                                                                              writeCounter(0),
                                                                                              totalCounter(0),
                                                                                              deterministic(det){

    }

    void StreamStoreman::store(std::vector<int64_t> *subseq, const std::string &stream_name) {
        std::vector<int64_t> buf;
        buf.swap(*subseq);

        // Todo: Check if this is possible without copying buf (reference to local var is undefined behavior in
        //  asynchronous execution)
        pool.pushTask([stream_name, this, buf]() mutable {

            // Pull ticket
            uint32_t ticket = 0;
            if (deterministic) {
                std::unique_lock<std::mutex> lock(stateLock);
                ticket = totalCounter++;
            }

            // Compress
            gabac::DataBlock compressed;
            compressor->compress(stream_name, &buf, &compressed);
            buf.clear();

            // Wait until it's out turn to write
            if(deterministic){
                std::unique_lock<std::mutex> lock(stateLock);
                if(writeCounter != ticket) {
                    wait_var.wait(lock, [&](){
                        return writeCounter == ticket;
                    });
                }
            }

            // Pack
            packer->pack(compressed, stream_name);

            // Notify next stream
            if (deterministic) {
                {
                    std::unique_lock<std::mutex> lock(stateLock);
                    writeCounter++;
                }
                wait_var.notify_all();
            }
        });
    }

    void StreamStoreman::wait() {
        pool.wait();
    }

}
#ifndef GENIE_STREAMSTOREMAN_H
#define GENIE_STREAMSTOREMAN_H

#include <atomic>
#include <vector>
#include <stdint.h>
#include <string>
#include <set>
#include <iostream>
#include <regex>
#include <map>
#include <filesystem/filesystem.hpp>

#include "thread_pool.h"
#include "gabac/streams.h"
#include "gabac/configuration.h"
#include "gabac/run.h"
#include "gabac/analysis.h"

namespace dsg {

    struct JobState{
        gabac::DataBlock data;
        std::string name;
        uint32_t ticket;

        JobState(gabac::DataBlock *d, const std::string &n, uint32_t t) : name(n), ticket(t) {
            data.swap(d);
        }

        JobState(const JobState& state) = delete;

        JobState(JobState&& state) noexcept : name(std::move(state.name)), ticket(state.ticket) {
            data.swap(&state.data);
        }

        JobState() : data (0,1), name(""), ticket(0){

        }

        JobState& operator= (JobState&& state) noexcept {
            data.swap(&state.data);
            name = std::move(state.name);
            ticket = state.ticket;
            return *this;
        }

        bool operator< (const JobState& state) const {
            return ticket < state.ticket;
        }
    };


    class GabacCompressor {
    public:

    };

    class StreamStoreman {
    public:
        std::vector<JobState> uncompressedJobs;
        std::vector<JobState> uncompressedJobsBlocked;
        std::vector<JobState> compressedJobs;

        std::vector <std::thread> threads;

        std::mutex stateLock;
        std::atomic<bool> running;

        FILE* fout;

        std::set<std::string> configsInProgress;
        std::map<std::string, gabac::EncodingConfiguration> configs;

        uint32_t writeTicket;
        uint32_t totalTicket;

        std::string configPath;

        void compress(JobState* state) {
            gabac::EncodingConfiguration enConf = configs.at(getConfigName(state->name));
            uint64_t insize = state->data.getRawSize();

            gabac::IBufferStream istream(&state->data, 0);
            gabac::DataBlock out(0,1);
            gabac::OBufferStream ostream(&out);

            // Configure gabac streams
            gabac::IOConfiguration
                    ioconf = {&istream, &ostream, std::min(uint64_t(insize), uint64_t(1000000000u)), &std::cout,
                              gabac::IOConfiguration::LogLevel::TRACE};

            gabac::run(ioconf, enConf, false);

            ostream.flush(&state->data);
        }

        static std::string getConfigName(const std::string &stream) {
            std::regex subseq("^subseq\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
            std::regex id("^id_1\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
            std::regex qual("^quality_1\\.[0-9]+$");

            if(std::regex_match(stream, qual)) {
                return "quality_1";
            }
            if(std::regex_match(stream, id)) {
                return stream.substr(0, stream.find_first_of('.')) + stream.substr(stream.find_last_of('.'), std::string::npos);
            }
            if(std::regex_match(stream, subseq)) {
                return stream.substr(0, stream.find_first_of('.')) + stream.substr(stream.find_last_of('.', stream.find_last_of('.') - 1), std::string::npos);
            }

            return "unknown";
        }

        void pack(const gabac::DataBlock &data, const std::string &stream_name) {
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

        void work () {
            while(running) {
                JobState job;
                std::string confname;
                bool packPossible = false;
                bool compressPossible = false;
                bool loadNecessary = false;

                // Get Job and determine next possible step
                {
                    std::unique_lock<std::mutex> guard(stateLock);

                    // Packing possible?
                    if(!compressedJobs.empty() && compressedJobs.front().ticket == writeTicket){
                        job = std::move(compressedJobs.front());
                        compressedJobs.erase(compressedJobs.begin());
                        packPossible = true;

                        // Compression possible?
                    } else if (!uncompressedJobs.empty()) {
                        auto pos = uncompressedJobs.end() - 1;
                        job = std::move(*(pos));
                        uncompressedJobs.erase(pos);
                        compressPossible = true;

                        // Job is blocked
                        confname = getConfigName(job.name);
                        if (configsInProgress.find(confname) != configsInProgress.end()) {
                            uncompressedJobsBlocked.emplace_back(std::move(job));
                            continue;
                        }

                        // Needs reload
                        if (configs.find(confname) == configs.end()) {
                            loadNecessary = true;
                            configsInProgress.insert(confname);
                        }
                    }
                }
                if (packPossible) {
                    pack(job.data, job.name);
                    job.data.clear();
                    std::unique_lock<std::mutex> guard(stateLock);
                    writeTicket++;
                    continue;
                }

                if (loadNecessary) {
                    if (!ghc::filesystem::exists(ghc::filesystem::path(configPath + confname + ".json"))) {
                        std::ofstream outstream(configPath + confname + ".json", std::ios::binary);
                        gabac::DataBlock d = job.data;
                        gabac::IBufferStream instream(&d);
                        gabac::IOConfiguration
                                ioconf{&instream, &outstream, std::min(uint64_t(d.getRawSize()), uint64_t(10000000u)),
                                       &std::cout, gabac::IOConfiguration::LogLevel::WARNING};

                        auto stream_params = getParams().find(confname);
                        if(stream_params == getParams().end()) {
                            GABAC_DIE("oops");
                        }
                        gabac::AnalysisConfiguration analyze_conf = gabac::getCandidateConfig();
                        analyze_conf.wordSize = stream_params->second.wordsize;
                        analyze_conf.maxValue = stream_params->second.maxval;

                        gabac::analyze(ioconf, analyze_conf);
                    }

                    // Load config from file
                    std::ifstream t(configPath + confname + ".json");
                    if (!t) {
                        throw std::runtime_error("Config not existent");
                    }
                    std::string configstring = std::string((std::istreambuf_iterator<char>(t)),
                                                           std::istreambuf_iterator<char>());

                    gabac::EncodingConfiguration enconf(configstring);

                    {
                        std::unique_lock<std::mutex> guard(stateLock);
                        configs.emplace(confname, enconf);
                        configsInProgress.erase(confname);

                        auto it = uncompressedJobsBlocked.begin();
                        while (it != uncompressedJobsBlocked.end()) {
                            if (getConfigName(it->name) == confname) {
                                uncompressedJobs.emplace_back(std::move(*it));
                                it = uncompressedJobsBlocked.erase(it);
                            } else {
                                ++it;
                            }
                        }
                    }
                }

                if(compressPossible) {
                    compress(&job);

                    {
                        std::unique_lock<std::mutex> guard(stateLock);

                        compressedJobs.insert(std::upper_bound(compressedJobs.begin(), compressedJobs.end(), job),
                                              std::move(job));
                    }
                }
            }
        }

        struct gabac_stream_params {
            uint64_t maxval;
            uint8_t wordsize;
        };

        static const std::map<std::string, gabac_stream_params>& getParams() {
            static const std::map<std::string, gabac_stream_params> mapping = {
                    {"subseq.0.0", {1022, 8}},
                    {"subseq.1.0", {1, 8}},
                    {"subseq.3.0", {1, 8}},
                    {"subseq.3.1", {511, 8}},
                    {"subseq.4.0", {1, 8}},
                    {"subseq.4.1", {4, 8}},
                    {"subseq.6.0", {4, 8}},
                    {"subseq.7.0", {std::numeric_limits<uint32_t >::max(), 8}},
                    {"subseq.8.0", {8, 8}},
                    {"subseq.8.1", {65535, 8}},
                    {"subseq.8.2", {512000, 8}},
                    {"subseq.8.3", {512000, 8}},
                    {"subseq.8.4", {std::numeric_limits<uint32_t >::max(), 8}},
                    {"subseq.8.5", {std::numeric_limits<uint32_t >::max(), 8}},
                    {"subseq.8.6", {512000, 8}},
                    {"subseq.8.7", {512000, 8}},
                    {"subseq.12.0", {5, 8}},
                    {"quality_1", {127, 1}},
                    {"id_1.0", {9, 8}},
                    {"id_1.1", {1, 8}},
                    {"id_1.2", {127, 8}},
                    {"id_1.3", {127, 8}},
                    {"id_1.4", {std::numeric_limits<uint32_t >::max(), 8}},
                    {"id_1.5", {255, 8}}

            };
            return mapping;
        }

    public:
        void store(const std::string &stream_name, gabac::DataBlock* data) {
            // Push back
            {
                std::unique_lock<std::mutex> guard(stateLock);
                uncompressedJobs.emplace_back(data, stream_name, totalTicket++);
            }
        }

        void wait () {
        }

        StreamStoreman(unsigned numThreads, const std::string& configp, FILE* f) : running(true), fout(f), writeTicket(0), totalTicket(0), configPath(configp){
            for (size_t i = 0; i < numThreads; ++i) {
                threads.emplace_back([this](){
                    this->work();
                });
            }
        }

        ~StreamStoreman() {
            wait();
            running = false;
            for (auto& t : threads) {
                t.join();
            }
        }

    };
}
#endif //GENIE_STREAMSTOREMAN_H

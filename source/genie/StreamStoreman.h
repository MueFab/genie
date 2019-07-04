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

        JobState(gabac::DataBlock *d, const std::string &n, uint32_t t);
        JobState(const JobState& state) = delete;

        JobState(JobState&& state) noexcept;
        JobState();

        JobState& operator= (JobState&& state) noexcept;

        bool operator< (const JobState& state) const;
    };

    class StreamStoreman {
    public:
        std::vector<JobState> uncompressedJobs;
        std::vector<JobState> uncompressedJobsBlocked;
        std::vector<JobState> compressedJobs;

        std::vector <std::thread> threads;

        std::mutex stateLock;
        std::atomic<bool> running;
        std::condition_variable wait_var;

        FILE* fout;

        std::set<std::string> configsInProgress;
        std::map<std::string, gabac::EncodingConfiguration> configs;

        uint32_t writeTicket;
        uint32_t totalTicket;

        std::string configPath;

        void compress(JobState* state);

        void pack(const gabac::DataBlock &data, const std::string &stream_name);

        void work ();

        struct gabac_stream_params {
            uint64_t maxval;
            uint8_t wordsize;
        };

        static const std::map<std::string, gabac_stream_params>& getParams();

    public:
        static std::string getConfigName(const std::string &stream);
        void store(const std::string &stream_name, gabac::DataBlock* data);
        void wait ();

        StreamStoreman(unsigned numThreads, const std::string& configp, FILE* f);
        ~StreamStoreman();

    };
}
#endif //GENIE_STREAMSTOREMAN_H

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

#include "thread_pool.h"
#include "gabac/streams.h"
#include "gabac/configuration.h"
#include "gabac/run.h"
#include "gabac/analysis.h"

namespace dsg {
    class StreamSaver {
    private:
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

        struct Action {
            JobState job;
            std::string confname;
            bool packPossible = false;
            bool compressPossible = false;
            bool loadNecessary = false;
        };

        std::vector<JobState> uncompressedJobs;         // Jobs that need to be compressed next
        std::vector<JobState> uncompressedJobsBlocked; /* Jobs that need to be compressed but are currently blocked
                                                         since the configuration is not yet available*/

        std::vector<JobState> compressedJobs;   // Jobs that need to be packed into the output file next

        std::vector <std::thread> threads; // Workers

        std::mutex stateLock; // Protection of internal state
        std::atomic<bool> running; // Flag that will stop workers, atomic to avoid locking the whole state for this

        std::condition_variable wait_var; // Will wait for all jobs to be done, used in wait();
        std::condition_variable sleep_var; // Will wait if there are no jobs available for a worker thread
        std::condition_variable insert_var; /* Will wait if the uncompressed queue is too crowded, avoid piling up of jobs
 *                                          leading to memory exhaustion*/

        std::ostream* fout; // Output file

        std::set<std::string> configsInProgress;  /* Set of configurations currently being loaded / generated,
 *                                                 avoiding to start generation multiple times in additional threads */
        std::map<std::string, gabac::EncodingConfiguration> configs; // Already loaded configurations

        uint32_t writeTicket;   // Contains the ID of the next job allowed to write to file - forces deterministic behavior
        uint32_t totalTicket;   // Contains the ID of the next job that will be added.

        std::string configPath; // Path to gabac config directory

        void compress(JobState* state); // Call gabac for job

        void pack(const gabac::DataBlock &data, const std::string &stream_name); // Pack stream into file

        void getNextAction(Action *a);  // Check if there is an action (containing a job + bonus information) available
        void generateConfIfNeeded(Action *a); // Regenerate conf if action indicates it is needed
        void packIfNeeded(Action *a); // Execute packing if action indicates it
        void loadConfIfNeeded(Action *a); // Load configuration if action tells so
        void compressIfNeeded(Action *a); // Compress job in action if action indicates

        void work (); // Working thread routine

        struct gabac_stream_params {
            uint64_t maxval; // Maximum value in stream
            uint8_t wordsize; // Maximum working word size
        };

        static const std::map<std::string, gabac_stream_params>& getParams(); // Get reference to hardcoded map.

    public:
        static std::string getConfigName(const std::string &stream);
        void store(const std::string &stream_name, gabac::DataBlock* data);
        void wait ();

        StreamSaver(unsigned numThreads, const std::string& configp, std::ostream* f);
        ~StreamSaver();

    };
}
#endif //GENIE_STREAMSAVER_H

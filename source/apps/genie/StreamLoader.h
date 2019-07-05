#ifndef GENIE_STREAMLOADER_H
#define GENIE_STREAMLOADER_H

#include <string>
#include <gabac/gabac.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <iostream>
#include <ureads-encoder/logger.h>
#include "StreamSaver.h"

namespace dsg {
    class StreamLoader {
    public:
        struct Streampos {
            std::streampos position;
            uint64_t size;
        };

        struct JobState {
            gabac::DataBlock *data;
            std::string name;
        };

        std::atomic<bool> running;
        std::vector<std::thread> threads;
        std::mutex stateLock;
        std::mutex fileLock;
        std::condition_variable sleep_var; // Will wait if there are no jobs available for a worker thread
        std::condition_variable wait_var; // Will wait if there are no jobs available for a worker thread
        std::istream *fin;
        std::string configPath;

        std::map<std::string, Streampos> file_index;
        std::vector<JobState> jobs_queue;
        size_t number_jobs;

        std::map<std::string, gabac::EncodingConfiguration> configs; // Already loaded configurations

        void work();

        void loadConfIfNeeded(const std::string &confname) ;


        void buildIndex();
        void unpack(JobState *job);

        void decompress(JobState *state);


    public:
        void load(const std::string &stream_name, gabac::DataBlock *data);

        void wait();

        StreamLoader(unsigned numThreads, const std::string &configp, std::istream *f);

        ~StreamLoader();
    };
}


#endif //GENIE_STREAMLOADER_H

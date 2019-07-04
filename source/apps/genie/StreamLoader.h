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

        void work() {
            while (running) {
                JobState job;
                {
                    std::unique_lock<std::mutex> guard(stateLock);
                    if(!jobs_queue.empty()) {
                        job = jobs_queue.front();
                        jobs_queue.erase(jobs_queue.begin());
                        loadConfIfNeeded(StreamSaver::getConfigName(job.name));
                    } else {
                        sleep_var.wait(guard, [this](){
                            return !jobs_queue.empty();
                        });
                        continue;
                    }
                }

                {
                    std::unique_lock<std::mutex> guard(fileLock);
                    unpack(&job);
                }
                decompress(&job);
                {
                    std::unique_lock<std::mutex> guard(stateLock);
                    number_jobs--;
                }
            }
        }

        void loadConfIfNeeded(const std::string &confname) {
            if(configs.find(confname) == configs.end()) {
                // Load config from file
                genie::Logger::instance().out("LOADING: " + configPath + confname + ".json " + "to " + confname);
                std::ifstream t(configPath + confname + ".json");
                if (!t) {
                    throw std::runtime_error("Config not existent");
                }
                std::string configstring = std::string((std::istreambuf_iterator<char>(t)),
                                                       std::istreambuf_iterator<char>());

                gabac::EncodingConfiguration enconf(configstring);
                configs.emplace(confname, enconf);
            }
        }


        void buildIndex() {
            fin->seekg(0, std::ios::seekdir::_S_beg);

            while (true) {
                uint64_t size;
                if (!fin->read(reinterpret_cast<char *>(&size), sizeof(uint64_t))) {
                    break;
                }

                std::string name(size, '\0');
                fin->read(const_cast<char *>(name.data()), size);

                fin->read(reinterpret_cast<char *>(&size), sizeof(uint64_t));

                std::streampos pos = fin->tellg();

                file_index[name] = {pos, size};

                std::cout << "NAME: " << name << " POSITION: " << file_index[name].position << " SIZE: "
                          << file_index[name].size << std::endl;

                fin->seekg(size, std::ios::seekdir::_S_cur);
            }
        }

        void unpack(JobState *job) {
            auto params = file_index[job->name];
            fin->seekg(params.position);
            job->data->setWordSize(1);
            job->data->resize(params.size);
            fin->read(reinterpret_cast<char *>(job->data->getData()), params.size);
        }

        void decompress(JobState *state) {
            gabac::EncodingConfiguration enConf = configs.at(StreamSaver::getConfigName(state->name));
            uint64_t insize = state->data->getRawSize();
            gabac::IBufferStream istream(state->data, 0);
            gabac::DataBlock out(0, 1);
            gabac::OBufferStream ostream(&out);

            // Configure gabac streams
            gabac::IOConfiguration
                    ioconf = {&istream, &ostream, insize, &std::cout,
                              gabac::IOConfiguration::LogLevel::TRACE};

            gabac::run(ioconf, enConf, true);

            ostream.flush(state->data);

            genie::Logger::instance().out(
                    "DECOMPRESS:" + state->name + " with CONFIG: " + StreamSaver::getConfigName(state->name) +
                    " from " + std::to_string(insize) + " to " +
                    std::to_string(state->data->getRawSize()));
        }


    public:
        void load(const std::string &stream_name, gabac::DataBlock *data) {
            std::unique_lock<std::mutex> guard(stateLock);
            number_jobs++;
            jobs_queue.push_back({data, stream_name});

        }

        void wait() {
            std::unique_lock<std::mutex> guard(stateLock);
            if(number_jobs) {
                wait_var.wait(guard, [this](){
                   return !number_jobs;
                });
            }
        }

        StreamLoader(unsigned numThreads, const std::string &configp, std::istream *f) : running(true), fin(f),
                                                                                         configPath(configp) {
            buildIndex();
            for (size_t i = 0; i < numThreads; ++i) {
                threads.emplace_back([this]() {
                    this->work();
                });
            }
        }

        ~StreamLoader() {
            wait();
            {
                std::unique_lock<std::mutex> guard(stateLock);
                running = false;
                sleep_var.notify_all();
            }
            for (auto &t : threads) {
                t.join();
            }
        }
    };
}


#endif //GENIE_STREAMLOADER_H

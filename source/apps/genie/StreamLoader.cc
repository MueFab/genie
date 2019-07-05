#include "StreamLoader.h"


namespace dsg {

    void StreamLoader::work() {
        while (running) {
            std::cout << "loop" << std::endl;
            JobState job;
            {
                std::unique_lock<std::mutex> guard(stateLock);
                if (!jobs_queue.empty()) {
                    job = jobs_queue.front();
                    jobs_queue.erase(jobs_queue.begin());
                    if(file_index[job.name].size) {
                        loadConfIfNeeded(StreamSaver::getConfigName(job.name));
                    }
                } else {
                    std::cout << "Sleep" << std::endl;
                    sleep_var.wait(guard, [this]() {
                        return !jobs_queue.empty() || !running;
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
                if(number_jobs == 0) {
                    wait_var.notify_all();
                }
            }
        }
    }

    void StreamLoader::loadConfIfNeeded(const std::string &confname) {
        if (configs.find(confname) == configs.end()) {
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


    void StreamLoader::buildIndex() {
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
        fin->clear();
    }

    void StreamLoader::unpack(JobState *job) {
        auto params = file_index[job->name];
        fin->seekg(params.position);
        job->data->setWordSize(1);
        job->data->resize(params.size);
        if(!fin->read(reinterpret_cast<char *>(job->data->getData()), params.size)) {
            GABAC_DIE("Error reading stream from file");
        }
    }

    void StreamLoader::decompress(JobState *state) {
        if(state->data->empty())
            return;
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


    void StreamLoader::load(const std::string &stream_name, gabac::DataBlock *data) {
        std::cout <<
                  "STREAM: " + stream_name + " with config " + StreamSaver::getConfigName(stream_name) << std::endl;
        std::unique_lock<std::mutex> guard(stateLock);
        number_jobs++;
        jobs_queue.push_back({data, stream_name});
       std::cout <<
                "STREAM: " + stream_name + " with config " + StreamSaver::getConfigName(stream_name) << std::endl;
       sleep_var.notify_one();
    }

    void StreamLoader::wait() {
        std::cout << "wait" << std::endl;
        std::unique_lock<std::mutex> guard(stateLock);
        std::cout << "wait2" << std::endl;
        if (number_jobs) {
            wait_var.wait(guard, [this]() {
                return !number_jobs;
            });
        }
    }

    StreamLoader::StreamLoader(unsigned numThreads, const std::string &configp, std::istream *f) : running(true),
                                                                                                   fin(f),
                                                                                                   configPath(configp),
                                                                                                   number_jobs(0){
        buildIndex();
         for (size_t i = 0; i < numThreads; ++i) {
              threads.emplace_back([this]() {
                  this->work();
              });
          }
    }

    StreamLoader::~StreamLoader() {
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
}

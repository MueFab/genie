#include "genie/StreamStoreman.h"

namespace dsg {


    JobState::JobState(gabac::DataBlock *d, const std::string &n, uint32_t t) : name(n), ticket(t) {
        data.swap(d);
    }


    JobState::JobState(JobState &&state) noexcept : name(std::move(state.name)), ticket(state.ticket) {
        data.swap(&state.data);
    }

    JobState::JobState() : data(0, 1), name(""), ticket(0) {

    }

    JobState &JobState::operator=(JobState &&state) noexcept {
        data.swap(&state.data);
        name = std::move(state.name);
        ticket = state.ticket;
        return *this;
    }

    bool JobState::operator<(const JobState &state) const {
        return ticket < state.ticket;
    }

    void StreamStoreman::compress(JobState *state) {
        std::cout << "COMPRESS:" << state->name << " with CONFIG: " << getConfigName(state->name) << std::endl;
        gabac::EncodingConfiguration enConf = configs.at(getConfigName(state->name));
        uint64_t insize = state->data.getRawSize();

        gabac::IBufferStream istream(&state->data, 0);
        gabac::DataBlock out(0, 1);
        gabac::OBufferStream ostream(&out);

        // Configure gabac streams
        gabac::IOConfiguration
                ioconf = {&istream, &ostream, std::min(uint64_t(insize), uint64_t(1000000000u)), &std::cout,
                          gabac::IOConfiguration::LogLevel::TRACE};

        gabac::run(ioconf, enConf, false);

        ostream.flush(&state->data);
    }

    std::string StreamStoreman::getConfigName(const std::string &stream) {
        std::regex subseq("^subseq\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
        std::regex id("^id_1\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
        std::regex qual("^quality_1\\.[0-9]+$");

        if (std::regex_match(stream, qual)) {
            return "quality_1";
        }
        if (std::regex_match(stream, id)) {
            return stream.substr(0, stream.find_first_of('.')) +
                   stream.substr(stream.find_last_of('.'), std::string::npos);
        }
        if (std::regex_match(stream, subseq)) {
            return stream.substr(0, stream.find_first_of('.')) +
                   stream.substr(stream.find_last_of('.', stream.find_last_of('.') - 1), std::string::npos);
        }

        return stream;
    }

    void StreamStoreman::pack(const gabac::DataBlock &data, const std::string &stream_name) {
        // Write name of input file
        uint64_t size = stream_name.size();

        std::cout << "PACKING: " << stream_name << std::endl;

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

        if (data.getRawSize()) {
            if (fwrite(data.getData(), data.getRawSize(), 1, fout) != 1) {
                throw std::runtime_error("Could not write to output file");
            }
        }
    }

    void StreamStoreman::work() {
        while (running) {
            JobState job;
            std::string confname;
            bool packPossible = false;
            bool compressPossible = false;
            bool loadNecessary = false;

            // Get Job and determine next possible step
            {
                std::unique_lock<std::mutex> guard(stateLock);

                // Packing possible?
                if (!compressedJobs.empty() && compressedJobs.front().ticket == writeTicket) {
                    job = std::move(compressedJobs.front());
                    compressedJobs.erase(compressedJobs.begin());
                    packPossible = true;

                    // Compression possible?
                } else if (!uncompressedJobs.empty()) {
                    auto pos = uncompressedJobs.begin();
                    job = std::move(*(pos));
                    uncompressedJobs.erase(pos);
                    compressPossible = true;

                    if (job.data.size() == 0) {
                        std::cout << "COMPRESS: SKIPPED" << job.name << " with CONFIG: " << getConfigName(job.name)
                                  << std::endl;
                        compressedJobs.insert(std::upper_bound(compressedJobs.begin(), compressedJobs.end(), job),
                                              std::move(job));
                        continue;
                    }

                    // Job is blocked
                    confname = getConfigName(job.name);
                    std::cout << "CONFIG NAME: " << confname << " for " << job.name << std::endl;
                    if (configsInProgress.find(confname) != configsInProgress.end()) {
                        uncompressedJobsBlocked.emplace_back(std::move(job));
                        std::cout << "BLOCKED: " << job.name << std::endl;
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
                wait_var.notify_all();
                continue;
            }

            if (loadNecessary) {
                if (!ghc::filesystem::exists(ghc::filesystem::path(configPath + confname + ".json"))) {
                    std::cout << "ANALYZING:" << configPath + confname + ".json" << std::endl;
                    std::ofstream outstream(configPath + confname + ".json", std::ios::binary);
                    gabac::DataBlock d = job.data;
                    size_t rawSize = d.getRawSize();
                    gabac::IBufferStream instream(&d);
                    gabac::IOConfiguration
                            ioconf{&instream, &outstream, std::min(uint64_t(rawSize), uint64_t(10000000u)),
                                   &std::cout, gabac::IOConfiguration::LogLevel::WARNING};

                    auto stream_params = getParams().find(confname);
                    if (stream_params == getParams().end()) {
                        GABAC_DIE("Config range " + confname + " for stream " + job.name + " not found");
                    }
                    gabac::AnalysisConfiguration analyze_conf = gabac::getCandidateConfig();
                    analyze_conf.wordSize = stream_params->second.wordsize;
                    analyze_conf.maxValue = stream_params->second.maxval;

                    gabac::analyze(ioconf, analyze_conf);
                }

                // Load config from file
                std::cout << "LOADING: " << configPath + confname + ".json " << "to " << confname << std::endl;
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

            if (compressPossible) {
                compress(&job);

                {
                    std::unique_lock<std::mutex> guard(stateLock);

                    compressedJobs.insert(std::upper_bound(compressedJobs.begin(), compressedJobs.end(), job),
                                          std::move(job));
                }
            }
        }
    }


    const std::map<std::string, StreamStoreman::gabac_stream_params> &StreamStoreman::getParams() {
        static const std::map<std::string, gabac_stream_params> mapping = {
                {"subseq.0.0",  {1022,                                 8}},
                {"subseq.1.0",  {1,                                    8}},
                {"subseq.3.0",  {1,                                    8}},
                {"subseq.3.1",  {511,                                  8}},
                {"subseq.4.0",  {1,                                    8}},
                {"subseq.4.1",  {4,                                    8}},
                {"subseq.6.0",  {4,                                    8}},
                {"subseq.7.0",  {std::numeric_limits<uint32_t>::max(), 8}},
                {"subseq.8.0",  {8,                                    8}},
                {"subseq.8.1",  {65535,                                8}},
                {"subseq.8.2",  {512000,                               8}},
                {"subseq.8.3",  {512000,                               8}},
                {"subseq.8.4",  {std::numeric_limits<uint32_t>::max(), 8}},
                {"subseq.8.5",  {std::numeric_limits<uint32_t>::max(), 8}},
                {"subseq.8.6",  {512000,                               8}},
                {"subseq.8.7",  {512000,                               8}},
                {"subseq.12.0", {5,                                    8}},
                {"quality_1",   {127,                                  1}},
                {"id_1.0",      {9,                                    8}},
                {"id_1.1",      {1,                                    8}},
                {"id_1.2",      {127,                                  8}},
                {"id_1.3",      {127,                                  8}},
                {"id_1.4",      {std::numeric_limits<uint32_t>::max(), 8}},
                {"id_1.5",      {255,                                  8}},
                {"cp.bin",      {255,                                  1}}

        };
        return mapping;
    }

    void StreamStoreman::store(const std::string &stream_name, gabac::DataBlock *data) {
        std::cout << "STREAM: " << stream_name << std::endl;
        // Push back
        {
            std::unique_lock<std::mutex> guard(stateLock);
            uncompressedJobs.emplace_back(data, stream_name, totalTicket++);
        }
    }

    void StreamStoreman::wait() {
        std::unique_lock<std::mutex> guard(stateLock);

        if (writeTicket == totalTicket)
            return;

        wait_var.wait(guard, [this]() {
            return writeTicket == totalTicket;
        });
    }

    StreamStoreman::StreamStoreman(unsigned numThreads, const std::string &configp, FILE *f) : running(true), fout(f),
                                                                                               writeTicket(0),
                                                                                               totalTicket(0),
                                                                                               configPath(configp) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this]() {
                this->work();
            });
        }
    }

    StreamStoreman::~StreamStoreman() {
        wait();
        running = false;
        for (auto &t : threads) {
            t.join();
        }
    }
}

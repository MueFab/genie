#include "genie/StreamSaver.h"

#include "ureads-encoder/logger.h"

namespace dsg {


    StreamSaver::JobState::JobState(gabac::DataBlock *d, const std::string &n, uint32_t t) : name(n), ticket(t) {
        data.swap(d);
    }


    StreamSaver::JobState::JobState(JobState &&state) noexcept : name(std::move(state.name)), ticket(state.ticket) {
        data.swap(&state.data);
    }

    StreamSaver::JobState::JobState() : data(0, 1), name(""), ticket(0) {

    }

    StreamSaver::JobState &StreamSaver::JobState::operator=(JobState &&state) noexcept {
        data.swap(&state.data);
        name = std::move(state.name);
        ticket = state.ticket;
        return *this;
    }

    bool StreamSaver::JobState::operator<(const JobState &state) const {
        return ticket < state.ticket;
    }

    void StreamSaver::compress(JobState *state) {
        gabac::EncodingConfiguration enConf = configs.at(getConfigName(state->name));
        uint64_t insize = state->data.getRawSize();

        gabac::IBufferStream istream(&state->data, 0);
        gabac::DataBlock out(0, 1);
        gabac::OBufferStream ostream(&out);

        // Configure gabac streams
        gabac::IOConfiguration
                ioconf = {&istream, &ostream, insize, &std::cout,
                          gabac::IOConfiguration::LogLevel::TRACE};

        gabac::run(ioconf, enConf, false);

        ostream.flush(&state->data);

        genie::Logger::instance().out("COMPRESS:" + state->name + " with CONFIG: " + getConfigName(state->name) +
        " from " + std::to_string(insize) + " to " + std::to_string(state->data.getRawSize()));
    }

    std::string StreamSaver::getConfigName(const std::string &stream) {
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

    void StreamSaver::pack(const gabac::DataBlock &data, const std::string &stream_name) {
        // Write name of input file
        uint64_t size = stream_name.size();

        genie::Logger::instance().out("PACKING: " + stream_name + " with size " + std::to_string(data.getRawSize()));

        fout->write(reinterpret_cast<char*>(&size), sizeof(uint64_t));
        fout->write(stream_name.c_str(), size);

        // Get and write size of input file
        size = data.getRawSize();
        fout->write(reinterpret_cast<char*>(&size), sizeof(uint64_t));

        if (size) {
            fout->write(reinterpret_cast<const char*>(data.getData()), data.getRawSize());
        }
    }

    void StreamSaver::getNextAction(dsg::StreamSaver::Action* a) {
        a->compressPossible = false;
        a->loadNecessary = false;
        a->packPossible = false;
        a->confname = "";
        a->job = JobState();

        std::unique_lock<std::mutex> guard(stateLock);

        // Packing possible?
        if (!compressedJobs.empty() && compressedJobs.front().ticket == writeTicket) {
            a->job = std::move(compressedJobs.front());
            compressedJobs.erase(compressedJobs.begin());
            a->packPossible = true;

            // Compression possible?
        } else if (!uncompressedJobs.empty()) {
            auto pos = uncompressedJobs.begin();
            a->job = std::move(*(pos));
            uncompressedJobs.erase(pos);

            if (a->job.data.size() == 0) {
                genie::Logger::instance().out("COMPRESS: SKIPPED" + a->job.name + " with CONFIG: " + getConfigName(a->job.name));
                compressedJobs.insert(std::upper_bound(compressedJobs.begin(), compressedJobs.end(), a->job),
                                      std::move(a->job));
                return;
            }

            // Job is blocked
            a->confname = getConfigName(a->job.name);
            genie::Logger::instance().out("CONFIG NAME: " + a->confname + " for " + a->job.name);
            if (configsInProgress.find(a->confname) != configsInProgress.end()) {
                uncompressedJobsBlocked.emplace_back(std::move(a->job));
                genie::Logger::instance().out("BLOCKED: " + a->job.name);
                return;
            }

            a->compressPossible = true;

            // Needs reload
            if (configs.find(a->confname) == configs.end()) {
                a->loadNecessary = true;
                configsInProgress.insert(a->confname);
            }
        } else {
            sleep_var.wait(guard);
        }
    }

    void StreamSaver::generateConfIfNeeded(Action *a) {
        if (!ghc::filesystem::exists(ghc::filesystem::path(configPath + a->confname + ".json"))) {
            genie::Logger::instance().out("ANALYZING:" + configPath + a->confname + ".json");
            std::ofstream outstream(configPath + a->confname + ".json", std::ios::binary);
            gabac::DataBlock d = a->job.data;
            size_t rawSize = d.getRawSize();
            gabac::IBufferStream instream(&d);
            gabac::IOConfiguration
                    ioconf{&instream, &outstream, std::min(uint64_t(rawSize), uint64_t(10000000u)),
                           &std::cout, gabac::IOConfiguration::LogLevel::WARNING};

            auto stream_params = getParams().find(a->confname);
            if (stream_params == getParams().end()) {
                GABAC_DIE("Config range " + a->confname + " for stream " + a->job.name + " not found");
            }
            gabac::AnalysisConfiguration analyze_conf = gabac::getCandidateConfig();
            analyze_conf.wordSize = stream_params->second.wordsize;
            analyze_conf.maxValue = stream_params->second.maxval;

            gabac::analyze(ioconf, analyze_conf);
        }
    }

    void StreamSaver::packIfNeeded(dsg::StreamSaver::Action *a) {
        if (a->packPossible) {
            pack(a->job.data, a->job.name);
            a->job.data.clear();
            std::unique_lock<std::mutex> guard(stateLock);
            writeTicket++;
            wait_var.notify_all();
        }
    }

    void StreamSaver::loadConfIfNeeded(dsg::StreamSaver::Action *a) {
        if (a->loadNecessary) {
            generateConfIfNeeded(a);

            // Load config from file
            genie::Logger::instance().out("LOADING: " + configPath + a->confname + ".json " + "to " + a->confname);
            std::ifstream t(configPath + a->confname + ".json");
            if (!t) {
                throw std::runtime_error("Config not existent");
            }
            std::string configstring = std::string((std::istreambuf_iterator<char>(t)),
                                                   std::istreambuf_iterator<char>());

            gabac::EncodingConfiguration enconf(configstring);

            {
                std::unique_lock<std::mutex> guard(stateLock);
                configs.emplace(a->confname, enconf);
                configsInProgress.erase(a->confname);

                auto it = uncompressedJobsBlocked.begin();
                while (it != uncompressedJobsBlocked.end()) {
                    if (getConfigName(it->name) == a->confname) {
                        uncompressedJobs.emplace_back(std::move(*it));
                        it = uncompressedJobsBlocked.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
    }

    void StreamSaver::compressIfNeeded(dsg::StreamSaver::Action *a) {
        if (a->compressPossible) {
            compress(&a->job);

            {
                std::unique_lock<std::mutex> guard(stateLock);

                compressedJobs.insert(std::upper_bound(compressedJobs.begin(), compressedJobs.end(), a->job),
                                      std::move(a->job));
            }
            sleep_var.notify_one();
            insert_var.notify_one();
        }
    }

    void StreamSaver::work() {
        while (running) {

            StreamSaver::Action a;
            // Get Job and determine next possible step
            getNextAction(&a);

            packIfNeeded(&a);
            loadConfIfNeeded(&a);
            compressIfNeeded(&a);
        }
    }


    const std::map<std::string, StreamSaver::gabac_stream_params> &StreamSaver::getParams() {
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

    void StreamSaver::store(const std::string &stream_name, gabac::DataBlock *data) {
        genie::Logger::instance().out("STREAM: " + stream_name + " with size " + std::to_string(data->getRawSize()));
        // Push back
        {
            std::unique_lock<std::mutex> guard(stateLock);
            const size_t MAX_QUEUE_SIZE = 4;
            if(uncompressedJobs.size() + compressedJobs.size() >= MAX_QUEUE_SIZE) {
                insert_var.wait(guard, [this](){
                    return uncompressedJobs.size() < MAX_QUEUE_SIZE;
                });
            }
            uncompressedJobs.emplace_back(data, stream_name, totalTicket++);
        }
        sleep_var.notify_one();
    }

    void StreamSaver::wait() {
        std::unique_lock<std::mutex> guard(stateLock);

        if (writeTicket == totalTicket)
            return;

        wait_var.wait(guard, [this]() {
            return writeTicket == totalTicket;
        });
    }

    StreamSaver::StreamSaver(unsigned numThreads, const std::string &configp, std::ostream *f) : running(true), fout(f),
                                                                                               writeTicket(0),
                                                                                               totalTicket(0),
                                                                                               configPath(configp) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this]() {
                this->work();
            });
        }
    }

    StreamSaver::~StreamSaver() {
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

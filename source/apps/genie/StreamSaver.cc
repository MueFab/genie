#include <cmath>
#include "genie/StreamSaver.h"

#include "ureads-encoder/logger.h"

namespace dsg {

    void StreamSaver::run_gabac(const std::string &name, gabac::DataBlock *data, bool decompression) {
        gabac::EncodingConfiguration enConf = configs.at(getConfigName(name));
        uint64_t insize = data->getRawSize();

        gabac::IBufferStream istream(data, 0);
        gabac::DataBlock out(0, 1);
        gabac::OBufferStream ostream(&out);

        // Configure gabac streams
        gabac::IOConfiguration
                ioconf = {&istream, &ostream, insize, &std::cout,
                          gabac::IOConfiguration::LogLevel::TRACE};

        gabac::run(ioconf, enConf, decompression);

        ostream.flush(data);

        genie::Logger::instance().out("COMPRESSED:" + name + " with CONFIG: " + getConfigName(name) +
                                      " from " + std::to_string(insize) + " to " + std::to_string(data->getRawSize()) + " in thread " + std::to_string(omp_get_thread_num()));
    }

    void StreamSaver::compress(const std::string &name, gabac::DataBlock *data) {
        run_gabac(name, data, false);
    }

    void StreamSaver::decompress(const std::string &name, gabac::DataBlock *data) {
        run_gabac(name, data, true);
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

        fout->write(reinterpret_cast<char *>(&size), sizeof(uint64_t));
        fout->write(stream_name.c_str(), size);

        // Get and write size of input file
        size = data.getRawSize();
        fout->write(reinterpret_cast<char *>(&size), sizeof(uint64_t));

        if (size) {
            fout->write(reinterpret_cast<const char *>(data.getData()), data.getRawSize());
        }
    }

    void StreamSaver::unpack(const std::string &stream_name, gabac::DataBlock *data) {
        auto position = file_index.find(stream_name);
        if(position == file_index.end()){
            data->clear();
            return;
        }
        data->resize(size_t(std::ceil(position->second.size / float(data->getWordSize()))));

        fin->seekg(position->second.position);
        fin->read(reinterpret_cast<char*>(data->getData()), position->second.size);

        genie::Logger::instance().out("UNPACKING: " + stream_name + " with size " + std::to_string(data->getRawSize()));

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

    StreamSaver::StreamSaver(const std::string &configp, std::ostream *f, std::istream *ifi) : fout(f), fin(ifi),
                                                                                               configPath(configp) {
        for (const auto &e : getParams()) {
            loadConfig(e.first);
        }
        if(fin) {
            buildIndex();
        }
    }

    void StreamSaver::buildIndex() {
#if defined(__APPLE__)
        fin->seekg(0, std::ios::seekdir::beg);
#else
        fin->seekg(0, std::ios::seekdir::_S_beg);
#endif

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
#if defined(__APPLE__)
            fin->seekg(size, std::ios::seekdir::cur);
#else
            fin->seekg(0, std::ios::seekdir::_S_cur);
#endif
        }
        fin->clear();

    }

    void StreamSaver::loadConfig(const std::string &name) {
        // Load config from file
        std::ifstream t(configPath + name + ".json");
        std::string configstring;
        if (t) {
            genie::Logger::instance().out("LOADING: " + configPath + name + ".json " + "to " + name);
            configstring = std::string((std::istreambuf_iterator<char>(t)),
                                       std::istreambuf_iterator<char>());
        } else {
            genie::Logger::instance().out("WARNING: Could not load config: " + name + " ; Falling back to default");
            configstring = getDefaultConf();
        }

        gabac::EncodingConfiguration enconf(configstring);

        configs.emplace(name, enconf);


    }

    StreamSaver::~StreamSaver() {
    }

    const std::string &StreamSaver::getDefaultConf() {
        static const std::string defaultGabacConf = "{\"word_size\":1,\"sequence_transformation_id\":0,\""
                                                    "sequence_transformation_parameter\":0,\"transformed_sequences\""
                                                    ":[{\"lut_transformation_enabled\":false,\"diff_coding_enabled\":"
                                                    "false,\"binarization_id\":0,\"binarization_parameters\":[8],\""
                                                    "context_selection_id\":1}]}";
        return defaultGabacConf;
    }
}

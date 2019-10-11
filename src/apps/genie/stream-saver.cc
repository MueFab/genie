#include <cmath>
#include <ios>

#include <genie/stream-saver.h>

#include <ureads-encoder/logger.h>

namespace dsg {

void StreamSaver::run_gabac(const std::string &name, gabac::DataBlock *data, bool decompression) {
    gabac::EncodingConfiguration enConf = configs.at(getConfigName(name));
    uint64_t insize = data->getRawSize();

    gabac::IBufferStream istream(data, 0);
    gabac::DataBlock out(0, 1);
    gabac::OBufferStream ostream(&out);

    // Configure gabac streams
    gabac::IOConfiguration ioconf = {&istream, &ostream, insize, &std::cout, gabac::IOConfiguration::LogLevel::TRACE};

    gabac::run(ioconf, enConf, decompression);

    ostream.flush(data);

    unsigned thread_num = 0;

#ifdef GENIE_USE_OPENMP
    thread_num = omp_get_thread_num();
#endif

    genie::Logger::instance().out("COMPRESSED:" + name + " with CONFIG: " + getConfigName(name) + " from " +
                                  std::to_string(insize) + " to " + std::to_string(data->getRawSize()) + " in thread " +
                                  std::to_string(thread_num));
}

void StreamSaver::analyze(const std::string &name, gabac::DataBlock *data) {
    std::string configname = getConfigName(name);
    uint64_t insize = data->getRawSize();

    gabac::IBufferStream istream(data, 0);
    gabac::DataBlock out(0, 1);
    std::ofstream outfile(this->configPath + configname + ".json");
    outfile.exceptions(std::ios::badbit | std::ios::failbit);

    // Configure gabac streams
    gabac::IOConfiguration ioconf = {&istream, &outfile, std::min(insize, uint64_t(10000000)), &std::cout,
                                     gabac::IOConfiguration::LogLevel::TRACE};

    auto aconf = gabac::getCandidateConfig();
    const auto &params = getParams();
    aconf.maxValue = params.at(configname).maxval;
    aconf.wordSize = params.at(configname).wordsize;

    genie::Logger::instance().out("ANALYZING: " + this->configPath + configname + ".json");
    gabac::analyze(ioconf, aconf);
}

void StreamSaver::compress(const std::string &name, gabac::DataBlock *data) {
    gabac::DataBlock original;
    gabac::DataBlock decompressed;
    if (debug) {
        original = *data;
    }
    try {
        run_gabac(name, data, false);
        if (debug) {
            decompressed = *data;
            run_gabac(name, &decompressed, true);
            decompressed.setWordSize(original.getWordSize());

            if (!(decompressed == original)) {
                genie::Logger::instance().out("GABAC FAIL DETECTED: " + name);
                std::ofstream origFile("gabac_" + name + "_original");
                origFile.write((char *)original.getData(), original.getRawSize());
                std::ofstream decFile("gabac_" + name + "_decoded");
                decFile.write((char *)decompressed.getData(), decompressed.getRawSize());
                std::ofstream encFile("gabac_" + name + "_encoded");
                encFile.write((char *)data->getData(), data->getRawSize());
            }
        }
    } catch (const gabac::Exception &e) {
        genie::Logger::instance().out("GABAC EXCEPTION DETECTED: " + name);
        std::ofstream origFile("gabac_" + name + "_original");
        origFile.write((char *)original.getData(), original.getRawSize());
        std::ofstream decFile("gabac_" + name + "_decoded");
        decFile.write((char *)decompressed.getData(), decompressed.getRawSize());
        std::ofstream encFile("gabac_" + name + "_encoded");
        encFile.write((char *)data->getData(), data->getRawSize());
        throw e;
    }
}

void StreamSaver::decompress(const std::string &name, gabac::DataBlock *data) { run_gabac(name, data, true); }

std::string StreamSaver::getConfigName(const std::string &stream) {
    std::regex subseq("^subseq\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
    std::regex id("^id_1\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
    std::regex qual("^quality_1\\.[0-9]+$");

    if (std::regex_match(stream, qual)) {
        return "quality_1";
    }
    if (std::regex_match(stream, id)) {
        return stream.substr(0, stream.find_first_of('.')) + stream.substr(stream.find_last_of('.'), std::string::npos);
    }
    if (std::regex_match(stream, subseq)) {
        return stream.substr(0, stream.find_first_of('.')) +
               stream.substr(stream.find_last_of('.', stream.find_last_of('.') - 1), std::string::npos);
    }

    return stream;
}

uint64_t StreamSaver::pack(const gabac::DataBlock &data, const std::string &stream_name) {
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
    return size;
}

void StreamSaver::unpack(const std::string &stream_name, gabac::DataBlock *data) {
    auto position = file_index.find(stream_name);
    if (position == file_index.end()) {
        data->clear();
        return;
    }

    data->setWordSize(1);
    data->resize(position->second.size);

    fin->seekg(position->second.position);
    fin->read(reinterpret_cast<char *>(data->getData()), position->second.size);

    genie::Logger::instance().out("UNPACKING: " + stream_name + " with size " + std::to_string(data->getRawSize()));
}

const std::map<std::string, StreamSaver::gabac_stream_params> &StreamSaver::getParams() {
    static const std::map<std::string, gabac_stream_params> mapping = {
        {"subseq.0.0", {1022, 8}},
        {"subseq.1.0", {1, 8}},
        {"subseq.3.0", {1, 8}},
        {"subseq.3.1", {511, 8}},
        {"subseq.4.0", {1, 8}},
        {"subseq.4.1", {4, 8}},
        {"subseq.6.0", {4, 8}},
        {"subseq.7.0", {std::numeric_limits<uint32_t>::max(), 8}},
        {"subseq.8.0", {8, 8}},
        {"subseq.8.1", {65535, 8}},
        {"subseq.8.2", {512000, 8}},
        {"subseq.8.3", {512000, 8}},
        {"subseq.8.4", {std::numeric_limits<uint32_t>::max(), 8}},
        {"subseq.8.5", {std::numeric_limits<uint32_t>::max(), 8}},
        {"subseq.8.6", {512000, 8}},
        {"subseq.8.7", {512000, 8}},
        {"subseq.12.0", {5, 8}},
        {"quality_1", {127, 1}},
        {"id_1.0", {9, 8}},
        {"id_1.1", {1, 8}},
        {"id_1.2", {127, 8}},
        {"id_1.3", {127, 8}},
        {"id_1.4", {std::numeric_limits<uint32_t>::max(), 8}},
        {"id_1.5", {255, 8}},
        {"cp.bin", {255, 1}}

    };
    return mapping;
}

StreamSaver::StreamSaver(const std::string &configp, std::ostream *f, std::istream *ifi, bool deb)
    : debug(deb), fout(f), fin(ifi), configPath(configp) {
    if (fin) {
        buildIndex();
    }
    reloadConfigSet();
}

void StreamSaver::buildIndex() {
    fin->seekg(0, std::ios::beg);

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

        std::cout << "NAME: " << name << " POSITION: " << file_index[name].position
                  << " SIZE: " << file_index[name].size << std::endl;

        fin->seekg(size, std::ios::cur);
    }
    fin->clear();
}

void StreamSaver::loadConfig(const std::string &name) {
    if (!this->fin) {
        // Load config from file
        std::ifstream t(configPath + name + ".json");
        std::string configstring;
        if (t) {
            genie::Logger::instance().out("LOADING: " + configPath + name + ".json " + "to " + name);
            configstring = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        } else {
            genie::Logger::instance().out("WARNING: Could not load config: " + name + " ; Falling back to default");
            configstring = getDefaultConf();
        }

        gabac::EncodingConfiguration enconf(configstring);

        configs.emplace(name, enconf);
    } else {
        gabac::DataBlock block(0, 1);
        unpack("conf_" + name + ".json", &block);
        std::string json(block.getRawSize(), ' ');
        std::memcpy(&json[0], block.getData(), block.getRawSize());
        gabac::EncodingConfiguration enconf(json);
        configs.emplace(name, enconf);
    }
}

void StreamSaver::reloadConfigSet() {
    configs.clear();
    for (const auto &e : getParams()) {
        loadConfig(e.first);
    }
}

uint64_t StreamSaver::finish() {
    off_t size = 0;
    for (const auto &e : getParams()) {
        std::string params = configs.at(e.first).toJsonString();
        gabac::DataBlock block(&params);
        size += pack(block, "conf_" + e.first + ".json");
    }
    return size;
}

StreamSaver::~StreamSaver() {}

const std::string &StreamSaver::getDefaultConf() {
    static const std::string defaultGabacConf =
        "{\"word_size\":1,\"sequence_transformation_id\":0,\""
        "sequence_transformation_parameter\":0,\"transformed_sequences\""
        ":[{\"lut_transformation_enabled\":false,\"diff_coding_enabled\":"
        "false,\"binarization_id\":0,\"binarization_parameters\":[8],\""
        "context_selection_id\":1}]}";
    return defaultGabacConf;
}
}  // namespace dsg

// Copyright 2018 The genie authors

#include "genie/gabac_integration.h"
#include "genie/thread_pool.h"

#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "boost/filesystem.hpp"
#include "gabac/gabac.h"

namespace dsg {

std::string defaultGabacConf = "{\"word_size\":\"1\",\"sequence_transformation_id\":\"0\",\""
                               "sequence_transformation_parameter\":\"0\",\"transformed_sequences\""
                               ":[{\"lut_transformation_enabled\":\"0\",\"lut_transformation_bits\""
                               ":\"0\",\"lut_transformation_order\":\"0\",\"diff_coding_enabled\":\""
                               "0\",\"binarization_id\":\"0\",\"binarization_parameters\":[\"8\"],\""
                               "context_selection_id\":\"2\"}]}";


static std::string getConfigForFile(const std::string& file) {
    std::string streamName = file.substr(file.find_last_of('/') + 1, std::string::npos);

    if (streamName.substr(0, 3) == "id.") {
        streamName = "id" + streamName.substr(streamName.find_first_of('.', 3), std::string::npos);
    } else {
        streamName = streamName.substr(0, streamName.find_last_of('.'));
    }

    std::string configpath = "../gabac_config/" +
                             streamName +
                             ".json";
    return configpath;
}

void compress_one_file(const std::string& file, bool decompress){
    std::string config;
    std::string configpath = getConfigForFile(file);

    // Read config or fall back to default
    try {
        std::ifstream t(configpath);
        if (!t) {
            throw std::runtime_error("Config not existent");
        }
        config = std::string((std::istreambuf_iterator<char>(t)),
                             std::istreambuf_iterator<char>());
    }
    catch (...) {
        std::cerr << "Problem reading gabac configuration " << configpath << " switching to default" << std::endl;
        config = defaultGabacConf;
    }

    // Open input/output files
    FILE *fin_desc = fopen(file.c_str(), "rb");
    FILE *fout_desc = fopen((file + ".gabac").c_str(), "wb");
    if (!fin_desc) {
        throw std::runtime_error("Could not open " + file);
    }
    if (!fout_desc) {
        throw std::runtime_error("Could not open " + file + ".gabac");
    }

    // Configure gabac streams
    gabac::FileInputStream fin(fin_desc);
    gabac::FileOutputStream fout(fout_desc);
    gabac::IOConfiguration
            ioconf = {&fin, &fout, 0, &std::cout, gabac::IOConfiguration::LogLevel::TRACE};
    gabac::EncodingConfiguration enConf(config);

    // Coding
    if (!decompress) {
        gabac::encode(ioconf, enConf);
        std::cout << "Sucessfully compressed ";
    } else {
        gabac::decode(ioconf, enConf);
        std::cout << "Sucessfully decompressed ";
    }

    // Close files
    fclose(fin_desc);
    fclose(fout_desc);

    // Replace input file with output
    std::cout << file << "\n(" << boost::filesystem::file_size(file) << " to\t";
    if (boost::filesystem::file_size(file) < 10000) {
        std::cout << "\t";
    }
    std::cout << boost::filesystem::file_size(file + ".gabac") << ")" << std::endl;
    std::remove(file.c_str());
    std::rename((file + ".gabac").c_str(), file.c_str());
}

void run_gabac(const std::vector<std::string>& files, bool decompress){
    const size_t NUM_THREADS = 1;
    ThreadPool pool(NUM_THREADS);
    for (const auto& file : files) {
        pool.pushTask(
                [&file, decompress]
                {
                    compress_one_file(file, decompress);
                }
        );
    }
    pool.wait();
}
}
// Copyright 2018 The genie authors

#include "genie/gabac_integration.h"
#include "genie/thread_pool.h"

#include <cmath>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "boost/filesystem.hpp"
#include "gabac/gabac.h"
#include "gabac/analysis.h"

namespace dsg {

void updateBin(gabac::EncodingConfiguration& c1, uint64_t max, unsigned index) {
    unsigned bits = 0;
    switch(c1.transformedSequenceConfigurations[index].binarizationId) {
        case gabac::BinarizationId::BI:
            bits = uint8_t (std::ceil(std::log2(max + 1)));
            bits = std::max(bits, c1.transformedSequenceConfigurations[index].binarizationParameters[0]);
            c1.transformedSequenceConfigurations[index].binarizationParameters = {bits};
            break;
        case gabac::BinarizationId::EG:
            if(max > std::numeric_limits<int32_t>::max()) {
                bits = uint8_t (std::ceil(std::log2(max + 1)));
                bits = std::max(bits, c1.transformedSequenceConfigurations[index].binarizationParameters[0]);
                c1.transformedSequenceConfigurations[index].binarizationParameters = {bits};
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::BI;
            }
            break;
        case gabac::BinarizationId::TU:
            if(max >= 32) {
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::TEG;
                c1.transformedSequenceConfigurations[index].binarizationParameters = {32};
            }
            break;
        default:
            return;
    }
}

gabac::EncodingConfiguration updateConfig (const gabac::EncodingConfiguration& c1, uint64_t max, unsigned wordsize) {
    gabac::EncodingConfiguration ret = c1;

    ret.wordSize = std::min(ret.wordSize, wordsize);

    unsigned bits = uint8_t (std::ceil(std::log2(max + 1)));

    ret.transformedSequenceConfigurations[0].lutBits = std::min(std::max(bits, ret.transformedSequenceConfigurations[0].lutBits), ret.wordSize*8);
    updateBin(ret, max, 0);

    const size_t MAX_LUT_SIZE = 1u << 20u;

    if(ret.transformedSequenceConfigurations[0].lutTransformationEnabled) {
        if (max > MAX_LUT_SIZE){
            ret.transformedSequenceConfigurations[0].lutTransformationEnabled = false;
        } else if(max > size_t(std::sqrt(MAX_LUT_SIZE))) {
            ret.transformedSequenceConfigurations[0].lutOrder = 0;
        } else if(max > size_t(std::pow(MAX_LUT_SIZE, 1.0/3.0))) {
            ret.transformedSequenceConfigurations[0].lutOrder = std::min(1u, ret.transformedSequenceConfigurations[0].lutOrder);
        }
    }

    switch(ret.sequenceTransformationId) {
        case gabac::SequenceTransformationId::equality_coding:
            ret.transformedSequenceConfigurations[1].lutBits = 1;
            updateBin(ret, 1, 1);
            break;
        case gabac::SequenceTransformationId::match_coding:
            bits = uint8_t (std::ceil(std::log2(ret.sequenceTransformationParameter + 1)));
            ret.transformedSequenceConfigurations[1].lutBits = bits;
            updateBin(ret, ret.sequenceTransformationParameter, 1);
            ret.transformedSequenceConfigurations[2].lutBits = 32;
            updateBin(ret, std::numeric_limits<uint32_t >::max(), 2);
            break;
        case gabac::SequenceTransformationId::rle_coding:
            ret.transformedSequenceConfigurations[1].lutBits = 32;
            updateBin(ret, std::numeric_limits<uint32_t >::max(), 1);
            break;
        default:
            break;
    }

    return ret;

}

std::string defaultGabacConf = "{\"word_size\":\"1\",\"sequence_transformation_id\":\"0\",\""
                               "sequence_transformation_parameter\":\"0\",\"transformed_sequences\""
                               ":[{\"lut_transformation_enabled\":\"0\",\"lut_transformation_bits\""
                               ":\"0\",\"lut_transformation_order\":\"0\",\"diff_coding_enabled\":\""
                               "0\",\"binarization_id\":\"0\",\"binarization_parameters\":[\"8\"],\""
                               "context_selection_id\":\"2\"}]}";


static std::string getConfigForFile(const std::string& file, const std::string& config) {
    std::string streamName = file.substr(file.find_last_of('/') + 1, std::string::npos);

    if (streamName.substr(0, 3) == "id.") {
        streamName = "id" + streamName.substr(streamName.find_first_of('.', 3), std::string::npos);
    } else {
        streamName = streamName.substr(0, streamName.find_last_of('.'));
    }

    std::string configpath = config + "/" +
                             streamName +
                             ".json";
    return configpath;
}

void compress_one_file(const std::string& file, const std::string& configfolder, bool decompress){
    std::string config;
    std::string configpath = getConfigForFile(file, configfolder);

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
    std::ifstream fin_desc(file, std::ios::binary);
    std::ofstream fout_desc(file + ".gabac", std::ios::binary);
    if (!fin_desc) {
        throw std::runtime_error("Could not open " + file);
    }
    if (!fout_desc) {
        throw std::runtime_error("Could not open " + file + ".gabac");
    }

    // Configure gabac streams
    gabac::IOConfiguration
            ioconf = {&fin_desc, &fout_desc, 1000000000, &std::cout, gabac::IOConfiguration::LogLevel::TRACE};
    gabac::EncodingConfiguration enConf(config);

    std::cout << file << " ..." << std::endl;

    gabac::run(ioconf, enConf, decompress);

    std::cout << "Gabac finished successfully" << std::endl;

    // Close files
    fin_desc.close();
    fout_desc.close();

    // Replace input file with output
    std::cout << file << "\n(" << boost::filesystem::file_size(file) << " to\t";
    if (boost::filesystem::file_size(file) < 10000) {
        std::cout << "\t";
    }
    std::cout << boost::filesystem::file_size(file + ".gabac") << ")" << std::endl;
    std::remove(file.c_str());
    std::rename((file + ".gabac").c_str(), file.c_str());
}

unsigned getWordsize(const std::string& file){
    size_t size = boost::filesystem::file_size(boost::filesystem::path(file));
    unsigned wordsize = 1;
    if(!(size % 2)) {
        wordsize = 2;
    }
    if(!(size % 4)) {
        wordsize = 4;
    }
    if(!(size % 8)) {
        wordsize = 8;
    }
    return wordsize;
}

uint64_t getMaxValue(const std::string& file, size_t wordsize){
    uint64_t max = 0;
    FILE* fp = fopen(file.c_str(), "rb");
    uint64_t buffer = 0;
    while(fread(&buffer, wordsize, 1, fp)) {
        if(buffer > max) {
            max = buffer;
        }
    }
    fclose(fp);
    return max;
}

void update_one_config(const std::string& file, const std::string& configpath){
    std::ifstream t(configpath);
    if (!t) {
        throw std::runtime_error("Config not existent");
    }
    std::string configstring = std::string((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
    gabac::EncodingConfiguration config(configstring);

    unsigned wordsize = std::min(config.wordSize, getWordsize(file));
    uint64_t maxval = getMaxValue(file, wordsize);

    // avoid binarizations for large values
    if(maxval >= std::numeric_limits<uint32_t >::max() && wordsize == 8) {
        wordsize = 4;
        maxval = getMaxValue(file, wordsize);
    }

    gabac::EncodingConfiguration newconfig = updateConfig(config, maxval, wordsize);

    std::cout << file <<" : " << "wordsize " << wordsize << ", max " << maxval << std::endl;

    if(config.toJsonString() == newconfig.toJsonString()) {
        return;
    }

    std::cout << "Updated configuration " << configpath << std::endl;
    configstring = newconfig.toJsonString();
    FILE* fp = fopen(configpath.c_str(), "wb");
    if(!fwrite(configstring.c_str(), configstring.length(), 1, fp)) {
        throw std::runtime_error("Could not write updated config");
    }
    fclose(fp);
}

void update_configs(const std::vector<std::string>& files, const std::string& config){

    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& file : files) {
        std::string configpath = getConfigForFile(file, config);
        if(boost::filesystem::exists(boost::filesystem::path(configpath))){
            update_one_config(file, configpath);
        } else {
            std::cout << "Config " << configpath << " missing! Regenerating..." << std::endl;
            {
                std::ofstream outstream(configpath, std::ios::binary);
                std::ifstream instream(file, std::ios::binary);
                gabac::IOConfiguration
                        ioconf{&instream, &outstream, 10000000, &std::cout, gabac::IOConfiguration::LogLevel::WARNING};
                gabac::EncodingConfiguration enconf;
                gabac::analyze(ioconf, gabac::getCandidateConfig());
            }

            update_one_config(file, configpath);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "Config check took " << duration.count() << " seconds!" << std::endl;

}

void run_gabac(const std::vector<std::string>& files, const std::string& config, bool decompress, size_t threads){

    if (!decompress) {
        std::cout << "Testing if config is compatible..." << std::endl;
        update_configs(files, config);
        std::cout << "Testing of configs finished!" << std::endl;
    }

  //  throw std::runtime_error("exit");

    const size_t NUM_THREADS = threads;
    ThreadPool pool(NUM_THREADS);
    for (const auto& file : files) {
        pool.pushTask(
                [&file, &config, decompress]
                {
                    compress_one_file(file, config, decompress);
                }
        );
    }
    pool.wait();

}


}
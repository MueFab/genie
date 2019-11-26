// Copyright 2018 The genie authors

/**
 *  @file generation.c
 *  @brief Main entry point for descriptor stream generation algorithms
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include "generation.h"
#include <condition_variable>
#include <filesystem@e980ed0/filesystem.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "spring/decompress.h"
#include "spring/spring.h"
#include "util/exceptions.h"
#include "util/fasta-file-reader.h"
#include "util/fasta-record.h"
#include "util/fastq-file-reader.h"
#include "util/fastq-record.h"
#include "util/sam-file-reader.h"
#include "util/sam-record.h"

namespace dsg {

static void generationFromFastq_SPRING(const ProgramOptions &programOptions, const std::string &filename) {
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Descriptor stream generation from FASTQ file" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    bool paired_end = false;
    // Initialize a FASTQ file reader.
    util::FastqFileReader fastqFileReader1(programOptions.inputFilePath);
    std::cout << "Calling SPRING" << std::endl;
    if (programOptions.inputFilePairPath.empty()) {
        spring::generate_streams_SPRING(&fastqFileReader1, &fastqFileReader1, programOptions.numThreads,
                                               paired_end, programOptions.workingDirectory, programOptions.analyze, filename,
                                               programOptions.preserve_order, !programOptions.discard_quality,
                                               !programOptions.discard_ids);
    } else {
        paired_end = true;
        util::FastqFileReader fastqFileReader2(programOptions.inputFilePairPath);
        spring::generate_streams_SPRING(&fastqFileReader1, &fastqFileReader2, programOptions.numThreads,
                                               paired_end, programOptions.workingDirectory, programOptions.analyze, filename,
                                               programOptions.preserve_order, !programOptions.discard_quality,
                                               !programOptions.discard_ids);
    }
}

static void generationFromFastq(const ProgramOptions &programOptions) {
    std::string filename = programOptions.outputFilePath;

    if (filename.empty()) {
        size_t slash = programOptions.inputFilePath.find_last_of('/');
        size_t dot = programOptions.inputFilePath.find_last_of('.');

        //
        // Find the base name of the input file name.
        // Ignore any dots directory path (before last slash).
        //
        if ((dot == std::string::npos) || ((slash != std::string::npos) && (dot <= slash))) {
            dot = programOptions.inputFilePath.back();
        } else {
            dot--;  // Base name ends before the last dot
        }

        if (slash != std::string::npos) {
            dot -= slash;  // dot is now length of base name
            slash++;       // base name starts after slash
        } else {
            slash = 0;  // base name starts at beginning of string
                        // no need to adjust dot
        }

        filename = programOptions.inputFilePath.substr(slash, dot) + ".genie";
    } else {
        // warn if string doesn't end in ".genie" ??
    }

    // std::ofstream output(filename);
    // output.exceptions(std::ios::badbit | std::ios::failbit);
    //
    // if (!output) {
    //     throw std::runtime_error("Could not open output file: " + filename);
    // }

//    dsg::StreamSaver store(programOptions.configPath, &output, nullptr, programOptions.gabacDebug);
//    auto generated_aus = generationFromFastq_SPRING(programOptions, store);
    generationFromFastq_SPRING(programOptions, filename);
}

void decompression_fastq(const ProgramOptions &programOptions) {
    // Open file and create tmp directory with random name
    std::ifstream in(programOptions.inputFilePath);
    if (!in) {
        throw std::runtime_error("Could not open input file: " + programOptions.inputFilePath);
    }
    std::string temp_dir;
    while (true) {
        std::string random_str = "tmp." + spring::random_string(10);
        temp_dir = programOptions.workingDirectory + "/" + random_str + '/';
        if (!ghc::filesystem::exists(temp_dir)) {
            break;
        }
    }
    if (!ghc::filesystem::create_directory(temp_dir)) {
        throw std::runtime_error("Cannot create temporary directory: " + temp_dir);
    }
    std::cout << "Temporary directory: " << temp_dir << "\n";

    dsg::StreamSaver saver(programOptions.configPath, nullptr, &in, programOptions.gabacDebug);

    bool paired = spring::decompress(temp_dir, &saver, programOptions.numThreads, programOptions.combine_pairs);

    std::cout << paired << std::endl;

    // Finish fastq
    std::string outname = programOptions.inputFilePath;
    outname = outname.substr(0, outname.find_last_of('.'));
    if (paired) {
        outname += "_decompressed_1.fastq";
        std::rename((temp_dir + "decompressed_1.fastq").c_str(), outname.c_str());
        outname = programOptions.inputFilePath;
        outname = outname.substr(0, outname.find_last_of('.'));
        outname += "_decompressed_2.fastq";
        std::rename((temp_dir + "decompressed_2.fastq").c_str(), outname.c_str());
    } else {
        outname += ".genie-decompressed.fastq";
        std::rename((temp_dir + "decompressed.fastq").c_str(), outname.c_str());
    }

    ghc::filesystem::remove_all(temp_dir);
}

void decompression(const ProgramOptions &programOptions) {
    if (programOptions.inputFileType == "GENIE") {
        util::FastqStats stats;
        std::chrono::steady_clock::time_point start_t;
        if (programOptions.verbose) {
            stats.enabled = true;
            start_t = std::chrono::steady_clock::now();
        }

        decompression_fastq(programOptions);

        if (stats.enabled) {
            stats.total_t = std::chrono::steady_clock::now() - start_t;
            stats.printDecompressionStats();
        }
    } else if (programOptions.inputFileType == "SGENIE") {
        //            decompression_sam(programOptions);
    } else {
        UTILS_DIE("Wrong input file type: " + programOptions.inputFilePath);
    }
}

void generation(const ProgramOptions &programOptions) {
    if (programOptions.inputFileType == "FASTQ") {
        util::FastqStats stats;
        std::chrono::steady_clock::time_point start_t;
        if (programOptions.verbose) {
            stats.enabled = true;
            start_t = std::chrono::steady_clock::now();
        }

        generationFromFastq(programOptions);

        if (stats.enabled) {
            stats.total_t = std::chrono::steady_clock::now() - start_t;
            stats.printCompressionStats();
        }
    } else if (programOptions.inputFileType == "SAM") {
        //            generationFromSam(programOptions);
    } else {
        UTILS_DIE("Wrong input file type: " + programOptions.inputFilePath);
    }
}

}  // namespace dsg

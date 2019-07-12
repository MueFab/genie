// Copyright 2018 The genie authors


/**
 *  @file generation.c
 *  @brief Main entry point for descriptor stream generation algorithms
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "genie/generation.h"

#include <iostream>
#include <string>
#include <vector>
// #include <utils/MPEGGFileCreation/MPEGGFileCreator.h>
// #include <fio/gabac_file.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <filesystem/filesystem.hpp>

#include "exceptions.h"
#include "utils/fasta-file-reader.h"
#include "utils/fasta-record.h"
#include "utils/fastq-file-reader.h"
#include "utils/fastq-record.h"
#include "utils/sam-file-reader.h"
#include "utils/sam-record.h"
#include "spring/spring.h"

//#include "alico/main.h"

namespace spring {
    bool decompress(const std::string &temp_dir, dsg::StreamSaver *ld);
}

namespace dsg {

    static generated_aus generationFromFastq_SPRING(
            const ProgramOptions &programOptions,
            dsg::StreamSaver& st
    ) {
        std::cout << std::string(80, '-') << std::endl;
        std::cout << "Descriptor stream generation from FASTQ file" << std::endl;
        std::cout << std::string(80, '-') << std::endl;

        bool paired_end = false;
        // Initialize a FASTQ file reader.
        utils::FastqFileReader fastqFileReader1(programOptions.inputFilePath);
        std::cout << "Calling SPRING" << std::endl;
        if (programOptions.inputFilePairPath.empty()) {
            return spring::generate_streams_SPRING(
                    &fastqFileReader1,
                    &fastqFileReader1,
                    programOptions.numThreads,
                    paired_end,
                    programOptions.workingDirectory,
                    programOptions.analyze,
                    st
            );
        } else {
            paired_end = true;
            utils::FastqFileReader fastqFileReader2(programOptions.inputFilePairPath);
            return spring::generate_streams_SPRING(
                    &fastqFileReader1,
                    &fastqFileReader2,
                    programOptions.numThreads,
                    paired_end,
                    programOptions.workingDirectory,
                    programOptions.analyze,
                    st
            );
        }
    }

    static void generationFromFastq(
            const ProgramOptions &programOptions
    ) {

        std::string outfile =
                (programOptions.inputFilePath.substr(0, programOptions.inputFilePath.find_last_of('.')) + ".genie");

        std::ofstream output(outfile);
        output.exceptions(std::ios::badbit | std::ios::failbit);

        if (!output) {
            throw std::runtime_error("Could not open output file");
        }

        dsg::StreamSaver store(programOptions.configPath, &output, nullptr);
        auto generated_aus = generationFromFastq_SPRING(programOptions, store);

    }

    void decompression_fastq(
            const ProgramOptions &programOptions
    ) {
        // Open file and create tmp directory with random name
        std::ifstream in(programOptions.inputFilePath);
        if (!in) {
            throw std::runtime_error("Could not open input file");
        }
        std::string temp_dir;
        while (true) {
            std::string random_str = "tmp." + spring::random_string(10);
            temp_dir = "./" + random_str + '/';
            if (!ghc::filesystem::exists(temp_dir)) {
                break;
            }
        }
        if (!ghc::filesystem::create_directory(temp_dir)) {
            throw std::runtime_error("Cannot create temporary directory.");
        }
        std::cout << "Temporary directory: " << temp_dir << "\n";

        dsg::StreamSaver saver(programOptions.configPath, nullptr, &in);

        bool paired = spring::decompress(temp_dir, &saver);

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


    void decompression(
            const ProgramOptions &programOptions
    ) {
        if (programOptions.inputFileType == "GENIE") {
            decompression_fastq(programOptions);
        } else if (programOptions.inputFileType == "SGENIE") {
//            decompression_sam(programOptions);
        } else {
            throwRuntimeError("wrong input file type");
        }
    }

    void generation(
            const ProgramOptions &programOptions
    ) {
        if (programOptions.inputFileType == "FASTQ") {
            generationFromFastq(programOptions);
        } else if (programOptions.inputFileType == "SAM") {
//            generationFromSam(programOptions);
        } else {
            throwRuntimeError("wrong input file type");
        }
    }


}  // namespace dsg

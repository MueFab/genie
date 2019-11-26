#include "program-options.h"
#include <util/file-reader.h>
#include <cli11@13becad/CLI11.hpp>
#include <filesystem@e980ed0/filesystem.hpp>
#include <iostream>
#include <map>
#include <set>
#include "util/exceptions.h"
#include "utilities.h"

namespace dsg {

ProgramOptions::ProgramOptions(int argc, char *argv[])
    : force(false),
      verbose(false),
      help(false),
      analyze(false),
      gabacDebug(false),
      workingDirectory("./"),
      numThreads(1),
      inputFilePath(""),
      configPath(""),
      inputFilePairPath(""),
      inputFileType(""),
      outputFilePath(""),
      idAlgorithm(""),
      qvAlgorithm(""),
      readAlgorithm("") {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() {
    // Nothing to do here.
}

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app(
        "Genie MPEG-G reference encoder\n\n"
        "Usage fastq encoding: \n genie [fastq] [optional: paired fastq] -c [Path to gabac config] [Optional: -t "
        "number of threads]\n"
        "Usage fastq decoding: \n genie [genie file] -c [Path to gabac config] [Optional: -t number of threads]\n"
        "Usage sam encoding: \n genie [sam] [fasta reference] -c [Path to gabac config] [Optional: -t number of "
        "threads]\n"
        "Usage sam decoding: \n genie [sgenie file] -c [Path to gabac config] [Optional: -t number of threads]\n");

    std::string filename = "default";
    app.add_option("input-file", inputFilePath,
                   "First input file - fastq in fastq mode, sam in sam mode, genie/sgenie file in decompression mode. "
                   "This argument is positional (first argument) and mandatory")
        ->mandatory(true);
    app.add_option("input-file-pair-path", inputFilePairPath,
                   "Second input file - paired fastq in fastq mode (optional), reference fasta file in sam mode "
                   "(mandatory). This argument is also positional.");

    configPath = "";
    app.add_option("-c,--config-file-path", configPath,
                   "Path to directory with gabac configurations. Missing configuratons will be regenerated, which may "
                   "take a while");

    app.add_option("-o,--output-file-path", outputFilePath,
                   "Output file name (optional). Default is <input_file_name>.genie");

    analyze = false;
    app.add_flag("-g,--generate-configuration", analyze, "Generate a new set of configurations");

    discard_ids = false;
    app.add_flag("--discard-ids", discard_ids, "Do not include read ids");

    discard_quality = false;
    app.add_flag("--discard-qualities", discard_quality, "Do not include qualities");

    preserve_order = false;
    app.add_flag("--preserve-order", preserve_order, "Do not reorder, preserve order but sacrifice compression ratio");

    combine_pairs = false;
    app.add_flag("--combine-pairs", combine_pairs, "Decompression: match read pairs");

    gabacDebug = false;
    app.add_flag("--gabac-debug", gabacDebug, "Check all gabac streams and write failing streams to hard drive");

    numThreads = 1;
    app.add_option("-t,--numThreads", numThreads,
                   "How many threads to launch for parallel execution of Genie. Default is 1.");

    workingDirectory = "./";
    app.add_option("--working-dir", workingDirectory,
                   "Directory to store temporary files. Must have sufficient space and write permissions. "
                   "genie/SPRING requires temporary storage close to original file size (except when preserve-order is "
                   "specified). "
                   "Default is ./");

    verbose = false;
    app.add_flag("-v,--verbose,--stats", verbose, "Print compression ratios & speed");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        throw std::runtime_error("Command line parsing failed:" + std::to_string(app.exit(e)));
    }

    validate();
}

void ProgramOptions::validate() {
    std::set<std::string>::iterator it;

#ifndef GENIE_USE_OPENMP
    if (numThreads > 1) {
        UTILS_DIE("Genie was built without OpenMP. Only one thread is supported");
    }
#endif

    if (inputFilePath.empty()) {
        UTILS_DIE("No input file path provided");
    }

    if (!common::fileExists(inputFilePath)) {
        UTILS_DIE("Input file does not exist: " + inputFilePath);
    }

    // if (configPath.empty()) {
    //     if (inputFilePath.substr(inputFilePath.find_last_of('.')) != ".genie") {
    //         UTILS_DIE("You need to pass a config directory when not in genie decompression mode!");
    //     }
    // } else if (configPath.back() != '/') {
    //     configPath += "/";
    //     if (!ghc::filesystem::exists(configPath) || !ghc::filesystem::is_directory(configPath)) {
    //         UTILS_DIE("Config dir does not exist: " + configPath);
    //     }
    // }

    if (!inputFilePairPath.empty()) {
        if (!common::fileExists(inputFilePairPath)) {
            UTILS_DIE("Input pair file does not exist: " + inputFilePairPath);
        }
        if (inputFilePairPath == inputFilePath) {
            UTILS_DIE("Same name for two input files: " + inputFilePath);
        }
        std::cout << "paired mode activated" << std::endl;
    }

    //
    // inputFileType
    //

    std::set<std::string> allowedInputFileTypes = {"GENIE", "SGENIE", "FASTQ", "SAM"};

    // Check if the user input string for inputFileType is in the set of
    // allowed input file types.

    inputFileType = inputFilePath.substr(inputFilePath.find_last_of('.') + 1, std::string::npos);

    std::transform(inputFileType.begin(), inputFileType.end(), inputFileType.begin(), ::toupper);

    if (allowedInputFileTypes.find(inputFileType) == allowedInputFileTypes.end()) {
        UTILS_DIE("Input file type is invalid: " + inputFilePath);
    }
}

}  // namespace dsg

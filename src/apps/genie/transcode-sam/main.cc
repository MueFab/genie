/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/main.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "apps/genie/transcode-sam/program-options.h"
#include "genie/core/flowgraph.h"
#include "genie/format/mgrec/exporter.h"
#include "genie/format/mgrec/importer.h"
#include "genie/format/sam/exporter.h"
#include "genie/format/sam/importer.h"
#include "genie/module/default-setup.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam {

std::string file_extension(const std::string& path) {
    auto pos = path.find_last_of('.');
    std::string ext = path.substr(pos + 1);
    for (auto& c : ext) {
        c = static_cast<char>(std::tolower(c));
    }
    return ext;
}

// ---------------------------------------------------------------------------------------------------------------------

enum class OperationCase { UNKNOWN = 0, CONVERT = 3 };

// ---------------------------------------------------------------------------------------------------------------------

enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

// ---------------------------------------------------------------------------------------------------------------------

FileType getType(const std::string& ext) {
    if (ext == "sam" || ext == "bam" || ext == "mgrec") {
        return FileType::THIRD_PARTY;
    } else if (ext == "mgb") {
        return FileType::MPEG;
    } else {
        return FileType::UNKNOWN;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

OperationCase getOperation(FileType in, FileType out) {
    if (in == FileType::THIRD_PARTY && out == FileType::THIRD_PARTY) {
        return OperationCase::CONVERT;
    } else {
        return OperationCase::UNKNOWN;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

OperationCase getOperation(const std::string& filenameIn, const std::string& filenameOut) {
    return getOperation(getType(file_extension(filenameIn)), getType(file_extension(filenameOut)));
}

template <class T>
void attachExporter(T& flow, const ProgramOptions& pOpts, std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
    std::ostream* file1 = &std::cout;
    if (file_extension(pOpts.outputFile) == "sam" || file_extension(pOpts.outputFile) == "bam") {
        UTILS_DIE_IF(pOpts.outputFile == "-.sam",
                     "Sam output to stdout currently not supported.");  // TODO(renzDef) add after refactoring importer
        UTILS_DIE_IF(pOpts.outputFile == "-.bam",
                     "Bam output to stdout currently not supported.");  // TODO(renzDef) add after refactoring importer
        flow.addExporter(std::make_unique<genie::format::sam::Exporter>(pOpts.fasta_file_path, pOpts.outputFile));
    } else if (file_extension(pOpts.outputFile) == "mgrec") {
        if (pOpts.outputFile.substr(0, 2) != "-.") {
            outputFiles.emplace_back(std::make_unique<std::ofstream>(pOpts.outputFile));
            file1 = outputFiles.back().get();
        }
        flow.addExporter(std::make_unique<genie::format::mgrec::Exporter>(*file1));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <class T>
void attachImporter(T& flow, const ProgramOptions& pOpts, std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
                    std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
    constexpr size_t BLOCKSIZE = 256000;
    if (file_extension(pOpts.inputFile) == "sam" || file_extension(pOpts.inputFile) == "bam") {
        UTILS_DIE_IF(pOpts.inputFile == "-.sam",
                     "Sam input from stdin currently not supported.");  // TODO(renzDef) add after refactoring importer
        UTILS_DIE_IF(pOpts.inputFile == "-.bam",
                     "Bam input from stdin currently not supported.");  // TODO(renzDef) add after refactoring importer
        flow.addImporter(
            std::make_unique<genie::format::sam::Importer>(BLOCKSIZE, pOpts.inputFile, pOpts.fasta_file_path));
    } else if (file_extension(pOpts.inputFile) == "mgrec") {
        auto tmpFile = pOpts.outputFile + ".unsupported.mgrec";
        outputFiles.emplace_back(std::make_unique<std::ofstream>(tmpFile));

        std::istream* file1 = &std::cin;
        if (pOpts.inputFile.substr(0, 2) != "-.") {
            inputFiles.emplace_back(std::make_unique<std::ifstream>(pOpts.inputFile));
            file1 = inputFiles.back().get();
        }

        flow.addImporter(
            std::make_unique<genie::format::mgrec::Importer>(BLOCKSIZE, *file1, *outputFiles.back(), false));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::FlowGraph> buildConverter(const ProgramOptions& pOpts,
                                                       std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
                                                       std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
    auto flow = genie::module::buildDefaultConverter(pOpts.num_threads);
    attachExporter(*flow, pOpts, outputFiles);
    attachImporter(*flow, pOpts, inputFiles, outputFiles);
    return flow;
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    try {
        genieapp::transcode_sam::ProgramOptions programOptions(argc, argv);
        if (programOptions.help) {
            return 0;
        }

        genie::util::Watch watch;
        std::unique_ptr<genie::core::FlowGraph> flowGraph;
        std::vector<std::unique_ptr<std::ifstream>> inputFiles;
        std::vector<std::unique_ptr<std::ofstream>> outputFiles;

        switch (getOperation(programOptions.inputFile, programOptions.outputFile)) {
            case OperationCase::UNKNOWN:
                UTILS_DIE("Unknown constellation of file name extensions - don't know which operation to perform.");
            case OperationCase::CONVERT: {
                flowGraph = buildConverter(programOptions, inputFiles, outputFiles);
                break;
            }
        }

        flowGraph->run();

        auto stats = flowGraph->getStats();
        stats.addDouble("time-total", watch.check());
        std::cerr << stats << std::endl;
    } catch (const genie::util::Exception& e) {
        std::cerr << "Genie error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::runtime_error& e) {
        std::cerr << "Std error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_sam

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

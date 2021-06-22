/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-fastq/main.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "apps/genie/transcode-fastq/program-options.h"
#include "genie/core/format-importer-null.h"
#include "genie/core/name-encoder-none.h"
#include "genie/format/fasta/exporter.h"
#include "genie/format/fasta/manager.h"
#include "genie/format/fastq/exporter.h"
#include "genie/format/fastq/importer.h"
#include "genie/format/mgb/exporter.h"
#include "genie/format/mgb/importer.h"
#include "genie/format/mgrec/exporter.h"
#include "genie/format/mgrec/importer.h"
#include "genie/module/default-setup.h"
#include "genie/quality/qvwriteout/encoder-none.h"
#include "genie/read/lowlatency/encoder.h"
#include "genie/util/watch.h"

// TODO(Fabian): For some reason, compilation on windows fails if we move this include further up. Investigate.
#include "filesystem/filesystem.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_fastq {

// ---------------------------------------------------------------------------------------------------------------------

std::string file_extension(const std::string& path) {
    auto pos = path.find_last_of('.');
    std::string ext = path.substr(pos + 1);
    for (auto& c : ext) {
        c = static_cast<char>(std::tolower(c));
    }
    return ext;
}

// ---------------------------------------------------------------------------------------------------------------------

enum class OperationCase { UNKNOWN = 0, CONVERT = 3};

// ---------------------------------------------------------------------------------------------------------------------

enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

// ---------------------------------------------------------------------------------------------------------------------

FileType getType(const std::string& ext) {
    if (ext == "fastq" || ext == "mgrec") {
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

// ---------------------------------------------------------------------------------------------------------------------

template <class T>
void attachExporter(T& flow, const ProgramOptions& pOpts, std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
    outputFiles.emplace_back(genie::util::make_unique<std::ofstream>(pOpts.outputFile));
    if (file_extension(pOpts.outputFile) == "fastq") {
        if (file_extension(pOpts.outputSupFile) == "fastq") {
            auto& file1 = *outputFiles.back();
            outputFiles.emplace_back(genie::util::make_unique<std::ofstream>(pOpts.outputSupFile));
            flow.addExporter(genie::util::make_unique<genie::format::fastq::Exporter>(file1, *outputFiles.back()));
        } else {
            flow.addExporter(genie::util::make_unique<genie::format::fastq::Exporter>(*outputFiles.back()));
        }
    } else if (file_extension(pOpts.outputFile) == "mgrec") {
        flow.addExporter(genie::util::make_unique<genie::format::mgrec::Exporter>(*outputFiles.back()));
    } else if (file_extension(pOpts.outputFile) == "fasta") {
        flow.addExporter(genie::util::make_unique<genie::format::fasta::Exporter>(
            &flow.getRefMgr(), outputFiles.back().get(), pOpts.numberOfThreads));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <class T>
void attachImporter(T& flow, const ProgramOptions& pOpts, std::vector<std::unique_ptr<std::ifstream>>& inputFiles) {
    constexpr size_t BLOCKSIZE = 10000;
    inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(pOpts.inputFile));
    if (file_extension(pOpts.inputFile) == "fastq") {
        if (file_extension(pOpts.inputSupFile) == "fastq") {
            auto& file1 = *inputFiles.back();
            inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(pOpts.inputSupFile));
            flow.addImporter(
                genie::util::make_unique<genie::format::fastq::Importer>(BLOCKSIZE, file1, *inputFiles.back()));
        } else {
            flow.addImporter(genie::util::make_unique<genie::format::fastq::Importer>(BLOCKSIZE, *inputFiles.back()));
        }
    } else if (file_extension(pOpts.inputFile) == "mgrec") {
        flow.addImporter(genie::util::make_unique<genie::format::mgrec::Importer>(BLOCKSIZE, *inputFiles.back()));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::FlowGraph> buildConverter(const ProgramOptions& pOpts,
                                                       std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
                                                       std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
    auto flow = genie::module::buildDefaultConverter(pOpts.numberOfThreads);
    attachExporter(*flow, pOpts, outputFiles);
    attachImporter(*flow, pOpts, inputFiles);
    return flow;
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);
    if (pOpts.help) {
        return 0;
    }
    genie::util::Watch watch;
    std::unique_ptr<genie::core::FlowGraph> flowGraph;
    std::vector<std::unique_ptr<std::ifstream>> inputFiles;
    std::vector<std::unique_ptr<std::ofstream>> outputFiles;
    switch (getOperation(pOpts.inputFile, pOpts.outputFile)) {
        case OperationCase::UNKNOWN:
            UTILS_DIE("Unknown constellation of file name extensions - don't know which operation to perform.");
            break;
        case OperationCase::CONVERT: {
            flowGraph = buildConverter(pOpts, inputFiles, outputFiles);
            break;
        }
    }

    flowGraph->run();

    auto stats = flowGraph->getStats();
    stats.addDouble("time-total", watch.check());
    std::cout << stats << std::endl;

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace transcode_fastq
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

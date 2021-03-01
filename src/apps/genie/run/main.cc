/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/run/main.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "apps/genie/run/program-options.h"
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
#include "genie/format/sam/exporter.h"
#include "genie/format/sam/importer.h"
#include "genie/module/default-setup.h"
#include "genie/quality/qvwriteout/encoder-none.h"
#include "genie/read/lowlatency/encoder.h"
#include "genie/util/watch.h"

// TODO(Fabian): For some reason, compilation on windows fails if we move this include further up. Investigate.
#include "filesystem/filesystem.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace run {

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

enum class OperationCase { UNKNOWN = 0, ENCODE = 1, DECODE = 2, CONVERT = 3, CAPSULATE = 4 };

// ---------------------------------------------------------------------------------------------------------------------

enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

// ---------------------------------------------------------------------------------------------------------------------

FileType getType(const std::string& ext) {
    if (ext == "sam" || ext == "fastq" || ext == "mgrec" || ext == "fasta") {
        return FileType::THIRD_PARTY;
    } else if (ext == "mgb") {
        return FileType::MPEG;
    } else {
        return FileType::UNKNOWN;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

OperationCase getOperation(FileType in, FileType out) {
    if (in == FileType::THIRD_PARTY && out == FileType::MPEG) {
        return OperationCase::ENCODE;
    } else if (in == FileType::MPEG && out == FileType::THIRD_PARTY) {
        return OperationCase::DECODE;
    } else if (in == FileType::THIRD_PARTY && out == FileType::THIRD_PARTY) {
        return OperationCase::CONVERT;
    } else if (in == FileType::MPEG && out == FileType::MPEG) {
        return OperationCase::CAPSULATE;
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
    if (file_extension(pOpts.outputFile) == "sam") {
        flow.addExporter(genie::util::make_unique<genie::format::sam::Exporter>(
            genie::format::sam::header::Header::createDefaultHeader(), *outputFiles.back()));
    } else if (file_extension(pOpts.outputFile) == "fastq") {
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

void addFasta(const std::string& fastaFile, genie::core::FlowGraphEncode* flow,
              std::vector<std::unique_ptr<std::ifstream>>& inputFiles) {
    std::string fai = fastaFile.substr(0, fastaFile.size() - 5) + "fai";
    auto fasta_file = genie::util::make_unique<std::ifstream>(fastaFile);
    if (!ghc::filesystem::exists(fai)) {
        std::cout << "Indexing " << fastaFile << " ..." << std::endl;
        std::ofstream fai_file(fai);
        genie::format::fasta::FastaReader::index(*fasta_file, fai_file);
    }
    auto fai_file = genie::util::make_unique<std::ifstream>(fai);
    inputFiles.push_back(std::move(fasta_file));
    inputFiles.push_back(std::move(fai_file));
    flow->addReferenceSource(genie::util::make_unique<genie::format::fasta::Manager>(
        **(inputFiles.rbegin() + 1), **inputFiles.rbegin(), &flow->getRefMgr()));
}

// ---------------------------------------------------------------------------------------------------------------------

template <class T>
void attachImporter(T& flow, const ProgramOptions& pOpts, std::vector<std::unique_ptr<std::ifstream>>& inputFiles) {
    constexpr size_t BLOCKSIZE = 10000;
    inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(pOpts.inputFile));
    if (file_extension(pOpts.inputFile) == "sam") {
        flow.addImporter(genie::util::make_unique<genie::format::sam::Importer>(BLOCKSIZE, *inputFiles.back()));
    } else if (file_extension(pOpts.inputFile) == "fastq") {
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
    } else if (file_extension(pOpts.inputFile) == "fasta") {
        flow.addImporter(genie::util::make_unique<genie::core::NullImporter>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::FlowGraph> buildEncoder(const ProgramOptions& pOpts,
                                                     std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
                                                     std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
    constexpr size_t BLOCKSIZE = 10000;
    genie::core::ClassifierRegroup::RefMode mode;
    if (pOpts.refMode == "none") {
        mode = genie::core::ClassifierRegroup::RefMode::NONE;
    } else if (pOpts.refMode == "full") {
        mode = genie::core::ClassifierRegroup::RefMode::FULL;
    } else {
        mode = genie::core::ClassifierRegroup::RefMode::RELEVANT;
    }
    if (file_extension(pOpts.inputFile) == "fasta") {
        mode = genie::core::ClassifierRegroup::RefMode::FULL;
    }
    auto flow = genie::module::buildDefaultEncoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE, mode,
                                                   pOpts.rawReference);
    if (file_extension(pOpts.inputFile) == "fasta") {
        addFasta(pOpts.inputFile, flow.get(), inputFiles);
    } else if (!pOpts.inputRefFile.empty()) {
        if (file_extension(pOpts.inputRefFile) == "fasta") {
            addFasta(pOpts.inputRefFile, flow.get(), inputFiles);
        } else {
            UTILS_DIE("Unknown reference format");
        }
    }
    outputFiles.emplace_back(genie::util::make_unique<std::ofstream>(pOpts.outputFile,std::ios::binary));
    flow->addExporter(genie::util::make_unique<genie::format::mgb::Exporter>(outputFiles.back().get()));
    attachImporter(*flow, pOpts, inputFiles);
    if (pOpts.qvMode == "none") {
        flow->setQVCoder(genie::util::make_unique<genie::quality::qvwriteout::NoneEncoder>(), 0);
    }
    if (pOpts.readNameMode == "none") {
        flow->setNameCoder(genie::util::make_unique<genie::core::NameEncoderNone>(), 0);
    }
    if (pOpts.lowLatency) {
        flow->setReadCoder(genie::util::make_unique<genie::read::lowlatency::Encoder>(), 2);
        flow->setReadCoder(genie::util::make_unique<genie::read::lowlatency::Encoder>(), 3);
    }
    return flow;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::FlowGraph> buildDecoder(const ProgramOptions& pOpts,
                                                     std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
                                                     std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
    constexpr size_t BLOCKSIZE = 10000;
    auto flow = genie::module::buildDefaultDecoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE);
    if (!pOpts.inputRefFile.empty()) {
        if (file_extension(pOpts.inputRefFile) == "fasta") {
            std::string fai = pOpts.inputRefFile.substr(0, pOpts.inputRefFile.size() - 5) + "fai";
            auto fasta_file = genie::util::make_unique<std::ifstream>(pOpts.inputRefFile);
            if (!ghc::filesystem::exists(fai)) {
                std::cout << "Indexing " << pOpts.inputRefFile << " ..." << std::endl;
                std::ofstream fai_file(fai);
                genie::format::fasta::FastaReader::index(*fasta_file, fai_file);
            }
            auto fai_file = genie::util::make_unique<std::ifstream>(fai);
            inputFiles.push_back(std::move(fasta_file));
            inputFiles.push_back(std::move(fai_file));
            flow->addReferenceSource(genie::util::make_unique<genie::format::fasta::Manager>(
                **(inputFiles.rbegin() + 1), **inputFiles.rbegin(), &flow->getRefMgr()));
        } else if (file_extension(pOpts.inputRefFile) == "mgb") {
            inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(pOpts.inputRefFile));
            flow->addImporter(genie::util::make_unique<genie::format::mgb::Importer>(
                *inputFiles.back(), &flow->getRefMgr(), flow->getRefDecoder(), true));
        }
    }
    inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(pOpts.inputFile,std::ios::binary));
    flow->addImporter(genie::util::make_unique<genie::format::mgb::Importer>(
        *inputFiles.back(), &flow->getRefMgr(), flow->getRefDecoder(), file_extension(pOpts.outputFile) == "fasta"));
    attachExporter(*flow, pOpts, outputFiles);
    return flow;
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
        case OperationCase::ENCODE:
            flowGraph = buildEncoder(pOpts, inputFiles, outputFiles);
            break;
        case OperationCase::DECODE:
            flowGraph = buildDecoder(pOpts, inputFiles, outputFiles);
            break;
        case OperationCase::CONVERT: {
            flowGraph = buildConverter(pOpts, inputFiles, outputFiles);
            break;
        }
        case OperationCase::CAPSULATE:
            UTILS_DIE("Encapsulation / Decapsulation not yet supported.");
            break;
    }

    flowGraph->run();

    auto stats = flowGraph->getStats();
    stats.addDouble("time-total", watch.check());
    std::cout << stats << std::endl;

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace run
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

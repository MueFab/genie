/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#define NOMINMAX
#include "apps/genie/run/main.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "apps/genie/run/program-options.h"
#include "genie/core/format-importer-null.h"
#include "genie/core/name-encoder-none.h"
//#include "genie/format/fasta/exporter.h"
//#include "genie/format/fasta/manager.h"
//#include "genie/format/fastq/exporter.h"
//#include "genie/format/fastq/importer.h"
#include "genie/format/mgb/exporter.h"
#include "genie/format/mgb/importer.h"
#include "genie/format/mgrec/exporter.h"
#include "genie/format/mgrec/importer.h"
#include "genie/module/default-setup.h"
//#include "genie/quality/calq/decoder.h"
//#include "genie/quality/calq/encoder.h"
//#include "genie/quality/qvwriteout/encoder-none.h"
//#include "genie/read/lowlatency/encoder.h"
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

enum class OperationCase { UNKNOWN = 0, ENCODE = 1, DECODE = 2, CAPSULATE = 4 };

// ---------------------------------------------------------------------------------------------------------------------

enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

// ---------------------------------------------------------------------------------------------------------------------

FileType getType(const std::string& ext) {
    if (ext == "mgrec" || ext == "fasta") {
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

//template <class T>
//void attachExporter(T& flow, const ProgramOptions& pOpts, std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
//    std::ostream* out_ptr = &std::cout;
//    if (pOpts.outputFile.substr(0, 2) != "-.") {
//        outputFiles.emplace_back(genie::util::make_unique<std::ofstream>(pOpts.outputFile));
//        out_ptr = outputFiles.back().get();
//    }
//    if (file_extension(pOpts.outputFile) == "mgrec") {
//        flow.addExporter(genie::util::make_unique<genie::format::mgrec::Exporter>(*out_ptr));
//    } else if (file_extension(pOpts.outputFile) == "fasta") {
//        flow.addExporter(genie::util::make_unique<genie::format::fasta::Exporter>(&flow.getRefMgr(), out_ptr,
//                                                                                  pOpts.numberOfThreads));
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

//void addFasta(const std::string& fastaFile, genie::core::FlowGraphEncode* flow,
//              std::vector<std::unique_ptr<std::ifstream>>& inputFiles) {
//    std::string fai = fastaFile.substr(0, fastaFile.find_last_of('.') + 1) + "fai";
//    std::string sha = fastaFile.substr(0, fastaFile.find_last_of('.') + 1) + "sha256";
//    auto fasta_file = genie::util::make_unique<std::ifstream>(fastaFile);
//    if (!ghc::filesystem::exists(fai)) {
//        std::cerr << "Indexing " << fastaFile << " ..." << std::endl;
//        std::ofstream fai_file(fai);
//        genie::format::fasta::FastaReader::index(*fasta_file, fai_file);
//    }
//    if (!ghc::filesystem::exists(sha)) {
//        std::cerr << "Calculaing hashes " << fastaFile << " ..." << std::endl;
//        std::ofstream sha_file(sha);
//        std::ifstream fai_file(fai);
//        genie::format::fasta::FaiFile fai_reader(fai_file);
//        genie::format::fasta::FastaReader::hash(fai_reader, *fasta_file, sha_file);
//    }
//    auto fai_file = genie::util::make_unique<std::ifstream>(fai);
//    auto sha_file = genie::util::make_unique<std::ifstream>(sha);
//    inputFiles.push_back(std::move(fasta_file));
//    inputFiles.push_back(std::move(fai_file));
//    inputFiles.push_back(std::move(sha_file));
//    flow->addReferenceSource(genie::util::make_unique<genie::format::fasta::Manager>(
//        **(inputFiles.rbegin() + 2), **(inputFiles.rbegin() + 1), **inputFiles.rbegin(), &flow->getRefMgr(),
//        fastaFile));
//}

// ---------------------------------------------------------------------------------------------------------------------

//template <class T>
//void attachImporter(T& flow, const ProgramOptions& pOpts, std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
//                    std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
//    constexpr size_t BLOCKSIZE = 128000;
//    std::istream* in_ptr = &std::cin;
//    if (pOpts.inputFile.substr(0, 2) != "-.") {
//        inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(pOpts.inputFile));
//        in_ptr = inputFiles.back().get();
//    }
//    if (file_extension(pOpts.inputFile) == "mgrec") {
//        outputFiles.emplace_back(genie::util::make_unique<std::ofstream>(pOpts.outputFile + ".unsupported.mgrec"));
//        flow.addImporter(
//            genie::util::make_unique<genie::format::mgrec::Importer>(BLOCKSIZE, *in_ptr, *outputFiles.back()));
//    } else if (file_extension(pOpts.inputFile) == "fasta") {
//        flow.addImporter(genie::util::make_unique<genie::core::NullImporter>());
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

//std::unique_ptr<genie::core::FlowGraph> buildEncoder(const ProgramOptions& pOpts,
//                                                     std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
//                                                     std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
//    constexpr size_t BLOCKSIZE = 128000;
//    genie::core::ClassifierRegroup::RefMode mode;
//    if (pOpts.refMode == "none") {
//        mode = genie::core::ClassifierRegroup::RefMode::NONE;
//    } else if (pOpts.refMode == "full") {
//        mode = genie::core::ClassifierRegroup::RefMode::FULL;
//    } else {
//        mode = genie::core::ClassifierRegroup::RefMode::RELEVANT;
//    }
//    if (file_extension(pOpts.inputFile) == "fasta") {
//        mode = genie::core::ClassifierRegroup::RefMode::FULL;
//    }
//    auto flow = genie::module::buildDefaultEncoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE, mode,
//                                                   pOpts.rawReference, pOpts.rawStreams);
//    if (file_extension(pOpts.inputFile) == "fasta") {
//        addFasta(pOpts.inputFile, flow.get(), inputFiles);
//    } else if (!pOpts.inputRefFile.empty()) {
//        if (file_extension(pOpts.inputRefFile) == "fasta" || file_extension(pOpts.inputRefFile) == "fa") {
//            addFasta(pOpts.inputRefFile, flow.get(), inputFiles);
//        } else {
//            UTILS_DIE("Unknown reference format");
//        }
//    }
//    std::ostream* out_ptr = &std::cout;
//    if (pOpts.outputFile.substr(0, 2) != "-.") {
//        outputFiles.emplace_back(genie::util::make_unique<std::ofstream>(pOpts.outputFile, std::ios::binary));
//        out_ptr = outputFiles.back().get();
//    }
//    flow->addExporter(genie::util::make_unique<genie::format::mgb::Exporter>(out_ptr));
//    attachImporter(*flow, pOpts, inputFiles, outputFiles);
//    if (pOpts.qvMode == "none") {
//        flow->setQVCoder(genie::util::make_unique<genie::quality::qvwriteout::NoneEncoder>(), 0);
//    }
//    if (pOpts.qvMode == "calq") {
//        flow->setQVCoder(genie::util::make_unique<genie::quality::calq::Encoder>(), 0);
//    }
//    if (pOpts.readNameMode == "none") {
//        flow->setNameCoder(genie::util::make_unique<genie::core::NameEncoderNone>(), 0);
//    }
//    if (pOpts.lowLatency) {
//        flow->setReadCoder(genie::util::make_unique<genie::read::lowlatency::Encoder>(pOpts.rawStreams), 3);
//        flow->setReadCoder(genie::util::make_unique<genie::read::lowlatency::Encoder>(pOpts.rawStreams), 4);
//    }
//    return flow;
//}

// ---------------------------------------------------------------------------------------------------------------------

//std::unique_ptr<genie::core::FlowGraph> buildDecoder(const ProgramOptions& pOpts,
//                                                     std::vector<std::unique_ptr<std::ifstream>>& inputFiles,
//                                                     std::vector<std::unique_ptr<std::ofstream>>& outputFiles) {
//    constexpr size_t BLOCKSIZE = 128000;
//    auto flow = genie::module::buildDefaultDecoder(pOpts.numberOfThreads, pOpts.workingDirectory,
//                                                   pOpts.combinePairsFlag, BLOCKSIZE);
//
//    std::string json_uri_path = pOpts.inputRefFile;
//    if (ghc::filesystem::exists(pOpts.inputFile + ".json") && ghc::filesystem::file_size(pOpts.inputFile + ".json")) {
//        genie::core::meta::Dataset data(nlohmann::json::parse(std::ifstream(pOpts.inputFile + ".json")));
//        if (data.getReference()) {
//            std::string uri =
//                dynamic_cast<const genie::core::meta::external_ref::Fasta&>(data.getReference()->getBase()).getURI();
//            std::string scheme = "file://";
//            UTILS_DIE_IF(uri.substr(0, scheme.length()) != scheme, "Unknown URI scheme: " + uri);
//            std::string path = uri.substr(scheme.length());
//            std::cerr << "Extracted reference URI: " << uri << std::endl;
//            if (ghc::filesystem::exists(path)) {
//                std::cerr << "Reference URI valid." << std::endl;
//                json_uri_path = path;
//            } else {
//                std::cerr << "Reference URI invalid. Falling back to CLI reference." << std::endl;
//            }
//        }
//    }
//    if (!json_uri_path.empty()) {
//        if (file_extension(json_uri_path) == "fasta" || file_extension(json_uri_path) == "fa") {
//            std::string fai = json_uri_path.substr(0, json_uri_path.find_last_of('.') + 1) + "fai";
//            std::string sha = json_uri_path.substr(0, json_uri_path.find_last_of('.') + 1) + "sha256";
//            auto fasta_file = genie::util::make_unique<std::ifstream>(json_uri_path);
//            if (!ghc::filesystem::exists(fai)) {
//                std::cerr << "Indexing " << json_uri_path << " ..." << std::endl;
//                std::ofstream fai_file(fai);
//                genie::format::fasta::FastaReader::index(*fasta_file, fai_file);
//            }
//            if (!ghc::filesystem::exists(sha)) {
//                std::cerr << "Calculating hashes " << json_uri_path << " ..." << std::endl;
//                std::ofstream sha_file(sha);
//                std::ifstream fai_file(fai);
//                genie::format::fasta::FaiFile fai_reader(fai_file);
//                genie::format::fasta::FastaReader::hash(fai_reader, *fasta_file, sha_file);
//            }
//            auto fai_file = genie::util::make_unique<std::ifstream>(fai);
//            auto sha_file = genie::util::make_unique<std::ifstream>(sha);
//            inputFiles.push_back(std::move(fasta_file));
//            inputFiles.push_back(std::move(fai_file));
//            inputFiles.push_back(std::move(sha_file));
//            flow->addReferenceSource(genie::util::make_unique<genie::format::fasta::Manager>(
//                **(inputFiles.rbegin() + 2), **(inputFiles.rbegin() + 1), **inputFiles.rbegin(), &flow->getRefMgr(),
//                json_uri_path));
//        } else if (file_extension(json_uri_path) == "mgb") {
//            inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(json_uri_path));
//            flow->addImporter(genie::util::make_unique<genie::format::mgb::Importer>(
//                *inputFiles.back(), &flow->getRefMgr(), flow->getRefDecoder(), true));
//        }
//    }
//    std::istream* in_ptr = &std::cin;
//    if (pOpts.inputFile.substr(0, 2) != "-.") {
//        inputFiles.emplace_back(genie::util::make_unique<std::ifstream>(pOpts.inputFile, std::ios::binary));
//        in_ptr = inputFiles.back().get();
//    }
//    flow->addImporter(genie::util::make_unique<genie::format::mgb::Importer>(
//        *in_ptr, &flow->getRefMgr(), flow->getRefDecoder(), file_extension(pOpts.outputFile) == "fasta"));
//    attachExporter(*flow, pOpts, outputFiles);
//    return flow;
//}

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
            // flowGraph = buildEncoder(pOpts, inputFiles, outputFiles);
            UTILS_DIE("Encode not yet supported.");
            break;
        case OperationCase::DECODE:
            // flowGraph = buildDecoder(pOpts, inputFiles, outputFiles);
            UTILS_DIE("Decode not yet supported.");
            break;
        case OperationCase::CAPSULATE:
            UTILS_DIE("Encapsulation / Decapsulation not yet supported.");
            break;
    }

    flowGraph->run();

//    if (getOperation(pOpts.inputFile, pOpts.outputFile) == OperationCase::ENCODE) {
//        std::ofstream jsonfile(pOpts.outputFile + ".json");
//        auto jsonstring = flowGraph->getMeta().toJson().dump(4);
//        jsonfile.write(jsonstring.data(), jsonstring.length());
//    }

    auto stats = flowGraph->getStats();
    stats.addDouble("time-total", watch.check());
    std::cerr << stats << std::endl;

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace run
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

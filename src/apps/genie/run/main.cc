
#include <genie/format/fasta/reader.h>
#include <genie/format/fastq/exporter.h>
#include <genie/format/fastq/importer.h>
#include <genie/format/mgb/exporter.h>
#include <genie/format/mgb/importer.h>
#include <genie/format/mgrec/exporter.h>
#include <genie/format/mgrec/importer.h>
#include <genie/format/sam/exporter.h>
#include <genie/format/sam/importer.h>
#include <genie/module/default-setup.h>
#include <atomic>
#include <fstream>
#include <functional>
#include <map>
#include <thread>
#include <utility>
#include "program-options.h"
namespace genieapp {
namespace run {

std::string file_extension(const std::string& path) {
    auto pos = path.find_last_of('.');
    std::string ext = path.substr(pos + 1);
    for (auto& c : ext) {
        c = std::tolower(c);
    }
    return ext;
}

enum class OperationCase { UNKNOWN = 0, ENCODE = 1, DECODE = 2, CONVERT = 3, CAPSULATE = 4 };

enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

FileType getType(const std::string& ext) {
    if (ext == "sam") {
        return FileType::THIRD_PARTY;
    } else if (ext == "fastq") {
        return FileType::THIRD_PARTY;
    } else if (ext == "mgrec") {
        return FileType::THIRD_PARTY;
    } else if (ext == "mgb") {
        return FileType::MPEG;
    } else {
        return FileType::UNKNOWN;
    }
}

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

OperationCase getOperation(const std::string& filenameIn, const std::string& filenameOut) {
    return getOperation(getType(file_extension(filenameIn)), getType(file_extension(filenameOut)));
}

int main(int argc, char* argv[]) {
 /*   std::ifstream in("GCF_000001405.39_GRCh38.p13_genomic.fasta");
    std::ifstream inFai("GCF_000001405.39_GRCh38.p13_genomic.fasta.fai");
    genie::format::fasta::FastaFile file(in, inFai);
    std::cout << file.loadSection("NC_012920.1", 16509, 16569) << std::endl;
    return 0; */
    ProgramOptions pOpts(argc, argv);
    genie::util::Watch watch;
    constexpr size_t BLOCKSIZE = 1000;
    std::unique_ptr<genie::core::FlowGraph> flowGraph;
    std::ifstream input_file(pOpts.inputFile);
    std::ofstream output_file(pOpts.outputFile);
    std::ofstream output_sup_file;
    std::ifstream input_sup_file;
    switch (getOperation(pOpts.inputFile, pOpts.outputFile)) {
        case OperationCase::UNKNOWN:
            UTILS_DIE("Unknown constellation of file name extensions - doesn't know which operation to perform.");
            break;
        case OperationCase::ENCODE: {
            auto flow = genie::module::buildDefaultEncoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE);
            if(file_extension(pOpts.inputFile) == "sam") {
                flow->addImporter(genie::util::make_unique<genie::format::sam::Importer>(BLOCKSIZE, input_file));
            } else if (file_extension(pOpts.inputFile) == "fastq") {
                if (file_extension(pOpts.inputSupFile) == "fastq") {
                    input_sup_file.open(pOpts.inputSupFile);
                    flow->addImporter(genie::util::make_unique<genie::format::fastq::Importer>(BLOCKSIZE, input_file, input_sup_file));
                } else {
                    flow->addImporter(genie::util::make_unique<genie::format::fastq::Importer>(BLOCKSIZE, input_file));
                }
            } else if (file_extension(pOpts.inputFile) == "mgrec") {
                flow->addImporter(genie::util::make_unique<genie::format::mgrec::Importer>(BLOCKSIZE, input_file));
            }
            flow->addExporter(genie::util::make_unique<genie::format::mgb::Exporter>(&output_file));
            flowGraph = std::move(flow);
        } break;
        case OperationCase::DECODE: {
            auto flow = genie::module::buildDefaultDecoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE);
            flow->addImporter(genie::util::make_unique<genie::format::mgb::Importer>(input_file));
            if(file_extension(pOpts.outputFile) == "sam") {
                flow->addExporter(genie::util::make_unique<genie::format::sam::Exporter>(
                    genie::format::sam::header::Header::createDefaultHeader(), output_file));
            } else if (file_extension(pOpts.outputFile) == "fastq"){
                if(file_extension(pOpts.outputSupFile) == "fastq") {
                    output_sup_file.open(pOpts.outputSupFile);
                    flow->addExporter(genie::util::make_unique<genie::format::fastq::Exporter>(output_file, output_sup_file));
                } else {
                    flow->addExporter(genie::util::make_unique<genie::format::fastq::Exporter>(output_file));
                }
            } else if (file_extension(pOpts.outputFile) == "mgrec") {
                flow->addExporter(genie::util::make_unique<genie::format::mgrec::Exporter>(output_file));
            }
            flowGraph = std::move(flow);
        } break;
            break;
        case OperationCase::CONVERT:
            UTILS_DIE("Conversion between third party formats not yet supported.");
            break;
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

}  // namespace run
}  // namespace genieapp
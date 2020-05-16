#include <gabac-compressor.h>
#include <gabac-decompressor.h>
#include <genie/core/entropy-decoder.h>
#include <genie/core/entropy-encoder.h>
#include <genie/core/format-exporter.h>
#include <genie/core/format-importer.h>
#include <genie/core/name-decoder.h>
#include <genie/core/read-decoder.h>
#include <genie/core/read-encoder.h>
#include <genie/format/mgb/exporter.h>
#include <genie/format/mgb/importer.h>
#include <genie/format/sam/exporter.h>
#include <genie/format/sam/importer.h>
#include <genie/name/tokenizer/decoder.h>
#include <genie/name/tokenizer/encoder.h>
#include <genie/quality/qvwriteout/decoder.h>
#include <genie/quality/qvwriteout/encoder.h>
#include <genie/read/localassembly/decoder.h>
#include <genie/read/localassembly/encoder.h>
#include <genie/read/refcoder/decoder.h>
#include <genie/read/refcoder/encoder.h>
#include <genie/read/spring/spring-encoder.h>
#include <genie/util/exceptions.h>
#include <genie/util/selector.h>
#include <genie/util/thread-manager.h>
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

enum class EncodingCase { UNALIGNED = 0, ALIGNED_WITH_REF = 1, ALIGNED_NO_REF = 2 };

enum class OperationCase { UNKNOWN = 0, ENCODE = 1, DECODE = 2, CONVERT = 3, CAPSULATE = 4 };

enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

FileType getType(const std::string& ext) {
    if (ext == "sam") {
        return FileType::THIRD_PARTY;
    } else if (ext == "fastq") {
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

class FlowGraph {
   public:
    virtual void run() {}
    virtual void stop(bool) {}
};

class FlowGraphEncode : public FlowGraph {
    genie::util::ThreadManager mgr;
    std::unique_ptr<genie::core::Classifier> classifier;

    std::vector<std::unique_ptr<genie::core::FormatImporter>> importers;

    std::vector<std::unique_ptr<genie::core::ReadEncoder>> readCoders;
    genie::util::Selector<genie::core::record::Chunk, genie::core::AccessUnitRaw> readSelector;

    std::vector<std::unique_ptr<genie::core::QVEncoder>> qvCoders;
    genie::util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded,
                              const genie::core::record::Chunk&>
        qvSelector;

    std::vector<std::unique_ptr<genie::core::NameEncoder>> nameCoders;
    genie::util::SideSelector<genie::core::NameEncoder, genie::core::AccessUnitRaw::Descriptor,
                              const genie::core::record::Chunk&>
        nameSelector;

    std::vector<std::unique_ptr<genie::core::EntropyEncoder>> entropyCoders;
    genie::util::Selector<genie::core::AccessUnitRaw, genie::core::AccessUnitPayload> entropySelector;

    std::vector<std::unique_ptr<genie::core::FormatExporterCompressed>> exporters;
    genie::util::SelectorHead<genie::core::AccessUnitPayload> exporterSelector;

   public:
    explicit FlowGraphEncode(size_t threads) : mgr(threads) {
        entropySelector.setDrain(&exporterSelector);
        readSelector.setDrain(&entropySelector);
    }

    void addImporter(std::unique_ptr<genie::core::FormatImporter> dat) {
        importers.emplace_back();
        setImporter(std::move(dat), importers.size() - 1);
    }

    void setClassifier(std::unique_ptr<genie::core::Classifier> _classifier) {
        classifier = std::move(_classifier);

        for (auto& i : importers) {
            i->setClassifier(classifier.get());
        }
    }

    void setImporter(std::unique_ptr<genie::core::FormatImporter> dat, size_t index) {
        importers[index] = std::move(dat);
        importers[index]->setDrain(&readSelector);
        importers[index]->setClassifier(classifier.get());
    }

    void addReadCoder(std::unique_ptr<genie::core::ReadEncoder> dat) {
        readCoders.emplace_back(std::move(dat));
        readSelector.addBranch(readCoders.back().get(), readCoders.back().get());
        readCoders.back()->setQVCoder(&qvSelector);
        readCoders.back()->setNameCoder(&nameSelector);
    }

    void setReadCoder(std::unique_ptr<genie::core::ReadEncoder> dat, size_t index) {
        readCoders[index] = std::move(dat);
        readSelector.setBranch(readCoders[index].get(), readCoders[index].get(), index);
        readCoders[index]->setQVCoder(&qvSelector);
        readCoders[index]->setNameCoder(&nameSelector);
    }

    void addEntropyCoder(std::unique_ptr<genie::core::EntropyEncoder> dat) {
        entropyCoders.emplace_back(std::move(dat));
        entropySelector.addBranch(entropyCoders.back().get(), entropyCoders.back().get());
    }

    void setEntropyCoder(std::unique_ptr<genie::core::EntropyEncoder> dat, size_t index) {
        entropyCoders[index] = std::move(dat);
        entropySelector.setBranch(entropyCoders[index].get(), entropyCoders[index].get(), index);
    }

    void addExporter(std::unique_ptr<genie::core::FormatExporterCompressed> dat) {
        exporters.emplace_back(std::move(dat));
        exporterSelector.add(exporters.back().get());
    }

    void setExporter(std::unique_ptr<genie::core::FormatExporterCompressed> dat, size_t index) {
        exporters[index] = std::move(dat);
        exporterSelector.set(exporters[index].get(), index);
    }

    void addNameCoder(std::unique_ptr<genie::core::NameEncoder> dat) {
        nameCoders.emplace_back(std::move(dat));
        nameSelector.addMod(nameCoders.back().get());
    }

    void setNameCoder(std::unique_ptr<genie::core::NameEncoder> dat, size_t index) {
        nameCoders[index] = std::move(dat);
        nameSelector.setMod(nameCoders[index].get(), index);
    }

    void addQVCoder(std::unique_ptr<genie::core::QVEncoder> dat) {
        qvCoders.emplace_back(std::move(dat));
        qvSelector.addMod(qvCoders.back().get());
    }

    void setQVCoder(std::unique_ptr<genie::core::QVEncoder> dat, size_t index) {
        qvCoders[index] = std::move(dat);
        qvSelector.setMod(qvCoders[index].get(), index);
    }

    void setReadCoderSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun) {
        readSelector.setOperation(fun);
    }

    void setQVSelector(std::function<size_t(const genie::core::record::Chunk&)> fun) {
        qvSelector.setSelection(std::move(fun));

        for (auto& r : readCoders) {
            r->setQVCoder(&qvSelector);
        }
    }

    void setNameSelector(std::function<size_t(const genie::core::record::Chunk&)> fun) {
        nameSelector.setSelection(std::move(fun));

        for (auto& r : readCoders) {
            r->setNameCoder(&nameSelector);
        }
    }

    void setEntropyCoderSelector(const std::function<size_t(const genie::core::AccessUnitRaw&)>& fun) {
        entropySelector.setOperation(fun);
    }

    void setExporterSelector(const std::function<size_t(const genie::core::AccessUnitPayload&)>& fun) {
        exporterSelector.setOperation(fun);
    }

    void run() override {
        std::vector<genie::util::OriginalSource*> imps;
        for (auto& i : importers) {
            imps.emplace_back(i.get());
        }
        mgr.setSource(std::move(imps));
        mgr.run();
    }

    void stop(bool abort) override { mgr.stop(abort); }
};

class FlowGraphDecode : public FlowGraph {
    genie::util::ThreadManager mgr;
    std::vector<std::unique_ptr<genie::core::FormatImporterCompressed>> importers;

    std::vector<std::unique_ptr<genie::core::EntropyDecoder>> entropyCoders;
    genie::util::Selector<genie::core::AccessUnitPayload, genie::core::AccessUnitRaw> entropySelector;

    std::vector<std::unique_ptr<genie::core::ReadDecoder>> readCoders;
    genie::util::Selector<genie::core::AccessUnitRaw, genie::core::record::Chunk> readSelector;

    std::vector<std::unique_ptr<genie::core::QVDecoder>> qvCoders;
    genie::util::SideSelector<genie::core::QVDecoder, std::vector<std::string>,
                              const genie::core::parameter::QualityValues&, const std::vector<std::string>&,
                              genie::core::AccessUnitRaw::Descriptor&>
        qvSelector;

    std::vector<std::unique_ptr<genie::core::NameDecoder>> nameCoders;
    genie::util::SideSelector<genie::core::NameDecoder, std::vector<std::string>,
                              genie::core::AccessUnitRaw::Descriptor&>
        nameSelector;

    std::vector<std::unique_ptr<genie::core::FormatExporter>> exporters;
    genie::util::SelectorHead<genie::core::record::Chunk> exporterSelector;

   public:
    explicit FlowGraphDecode(size_t threads) : mgr(threads) {
        entropySelector.setDrain(&readSelector);
        readSelector.setDrain(&exporterSelector);
    }

    void addImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat) {
        importers.emplace_back();
        setImporter(std::move(dat), importers.size() - 1);
    }

    void setImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat, size_t index) {
        importers[index] = std::move(dat);
        importers[index]->setDrain(&entropySelector);
    }

    void addReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat) {
        readCoders.emplace_back(std::move(dat));
        readSelector.addBranch(readCoders.back().get(), readCoders.back().get());
        readCoders.back()->setQVCoder(&qvSelector);
        readCoders.back()->setNameCoder(&nameSelector);
    }

    void setReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat, size_t index) {
        readCoders[index] = std::move(dat);
        readSelector.setBranch(readCoders[index].get(), readCoders[index].get(), index);
        readCoders[index]->setQVCoder(&qvSelector);
        readCoders[index]->setNameCoder(&nameSelector);
    }

    void addEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat) {
        entropyCoders.emplace_back(std::move(dat));
        entropySelector.addBranch(entropyCoders.back().get(), entropyCoders.back().get());
    }

    void setEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat, size_t index) {
        entropyCoders[index] = std::move(dat);
        entropySelector.setBranch(entropyCoders[index].get(), entropyCoders[index].get(), index);
    }

    void addExporter(std::unique_ptr<genie::core::FormatExporter> dat) {
        exporters.emplace_back(std::move(dat));
        exporterSelector.add(exporters.back().get());
    }

    void setExporter(std::unique_ptr<genie::core::FormatExporter> dat, size_t index) {
        exporters[index] = std::move(dat);
        exporterSelector.set(exporters[index].get(), index);
    }

    void setReadCoderSelector(const std::function<size_t(const genie::core::AccessUnitRaw&)>& fun) {
        readSelector.setOperation(fun);
    }

    void setQVSelector(
        std::function<size_t(const genie::core::parameter::QualityValues& param, const std::vector<std::string>& ecigar,
                             genie::core::AccessUnitRaw::Descriptor& desc)>
            fun) {
        qvSelector.setSelection(std::move(fun));

        for (auto& r : readCoders) {
            r->setQVCoder(&qvSelector);
        }
    }

    void setNameSelector(std::function<size_t(genie::core::AccessUnitRaw::Descriptor& desc)> fun) {
        nameSelector.setSelection(std::move(fun));

        for (auto& r : readCoders) {
            r->setNameCoder(&nameSelector);
        }
    }

    void setEntropyCoderSelector(const std::function<size_t(const genie::core::AccessUnitPayload&)>& fun) {
        entropySelector.setOperation(fun);
    }

    void setExporterSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun) {
        exporterSelector.setOperation(fun);
    }

    void addNameCoder(std::unique_ptr<genie::core::NameDecoder> dat) {
        nameCoders.emplace_back(std::move(dat));
        nameSelector.addMod(nameCoders.back().get());
    }

    void setNameCoder(std::unique_ptr<genie::core::NameDecoder> dat, size_t index) {
        nameCoders[index] = std::move(dat);
        nameSelector.setMod(nameCoders[index].get(), index);
    }

    void addQVCoder(std::unique_ptr<genie::core::QVDecoder> dat) {
        qvCoders.emplace_back(std::move(dat));
        qvSelector.addMod(qvCoders.back().get());
    }

    void setQVCoder(std::unique_ptr<genie::core::QVDecoder> dat, size_t index) {
        qvCoders[index] = std::move(dat);
        qvSelector.setMod(qvCoders[index].get(), index);
    }

    void run() override {
        std::vector<genie::util::OriginalSource*> imps;
        for (auto& i : importers) {
            imps.emplace_back(i.get());
        }
        mgr.setSource(std::move(imps));
        mgr.run();
    }
};

std::unique_ptr<FlowGraphEncode> buildDefaultEncoder(size_t threads, const std::string& working_dir, size_t au_size) {
    std::unique_ptr<FlowGraphEncode> ret = genie::util::make_unique<FlowGraphEncode>(threads);

    ret->setClassifier(genie::util::make_unique<genie::core::ClassifierBypass>());

    ret->addReadCoder(genie::util::make_unique<genie::read::spring::SpringEncoder>(working_dir, nullptr));
    ret->addReadCoder(genie::util::make_unique<genie::read::localassembly::Encoder>(2048, false));
    ret->addReadCoder(genie::util::make_unique<genie::read::refcoder::Encoder>());
    ret->setReadCoderSelector([](const genie::core::record::Chunk&) -> size_t { return 1; });

    ret->addQVCoder(genie::util::make_unique<genie::quality::qvwriteout::Encoder>());
    ret->setQVSelector([](const genie::core::record::Chunk&) -> size_t { return 0; });

    ret->addNameCoder(genie::util::make_unique<genie::name::tokenizer::Encoder>());
    ret->setNameSelector([](const genie::core::record::Chunk&) -> size_t { return 0; });

    ret->addEntropyCoder(genie::util::make_unique<genie::entropy::gabac::GabacCompressor>());
    ret->setEntropyCoderSelector([](const genie::core::AccessUnitRaw&) -> size_t { return 0; });

    ret->setExporterSelector([](const genie::core::AccessUnitPayload&) -> size_t { return 0; });

    return ret;
}

std::unique_ptr<FlowGraphDecode> buildDefaultDecoder(size_t threads, const std::string& working_dir, size_t au_size) {
    std::unique_ptr<FlowGraphDecode> ret = genie::util::make_unique<FlowGraphDecode>(threads);

    // ret->addReadCoder(genie::util::make_unique<genie::read::spring::Decoder>(working_dir, nullptr));
    ret->addReadCoder(genie::util::make_unique<genie::read::localassembly::Decoder>());
    ret->addReadCoder(genie::util::make_unique<genie::read::localassembly::Decoder>());
    ret->addReadCoder(genie::util::make_unique<genie::read::refcoder::Decoder>());
    ret->setReadCoderSelector([](const genie::core::AccessUnitRaw&) -> size_t { return 1; });

    ret->addQVCoder(genie::util::make_unique<genie::quality::qvwriteout::Decoder>());
    ret->setQVSelector([](const genie::core::parameter::QualityValues& param, const std::vector<std::string>& ecigar,
                          genie::core::AccessUnitRaw::Descriptor& desc) -> size_t { return 0; });

    ret->addNameCoder(genie::util::make_unique<genie::name::tokenizer::Decoder>());
    ret->setNameSelector([](const genie::core::AccessUnitRaw::Descriptor&) -> size_t { return 0; });

    ret->addEntropyCoder(genie::util::make_unique<genie::entropy::gabac::GabacDecompressor>());
    ret->setEntropyCoderSelector([](const genie::core::AccessUnitPayload&) -> size_t { return 0; });

    ret->setExporterSelector([](const genie::core::record::Chunk&) -> size_t { return 0; });

    return ret;
}

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    constexpr size_t BLOCKSIZE = 10000;
    std::unique_ptr<FlowGraph> flowGraph;
    std::ifstream input_file(pOpts.inputFile);
    std::ofstream output_file(pOpts.outputFile);
    switch (getOperation(pOpts.inputFile, pOpts.outputFile)) {
        case OperationCase::UNKNOWN:
            UTILS_DIE("Unknown constellation of file name extensions - doesn't know which operation to perform.");
            break;
        case OperationCase::ENCODE: {
            auto flow = buildDefaultEncoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE);
            flow->addImporter(genie::util::make_unique<genie::format::sam::Importer>(BLOCKSIZE, input_file));
            flow->addExporter(genie::util::make_unique<genie::format::mgb::Exporter>(&output_file));
            flowGraph = std::move(flow);
        } break;
        case OperationCase::DECODE: {
            auto flow = buildDefaultDecoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE);
            flow->addImporter(genie::util::make_unique<genie::format::mgb::Importer>(input_file));
            flow->addExporter(genie::util::make_unique<genie::format::sam::Exporter>(
                genie::format::sam::header::Header::createDefaultHeader(), output_file));
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

    return 0;
}

}  // namespace run
}  // namespace genieapp
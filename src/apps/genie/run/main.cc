#include <gabac-compressor.h>
#include <genie/core/entropy-encoder.h>
#include <genie/core/format-exporter.h>
#include <genie/core/format-importer.h>
#include <genie/core/read-encoder.h>
#include <genie/format/mgb/exporter.h>
#include <genie/format/sam/importer.h>
#include <genie/name/tokenizer/encoder.h>
#include <genie/quality/qvwriteout/encoder.h>
#include <genie/read/localassembly/encoder.h>
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
    std::string ext = path.substr(pos);
    for (auto& c : ext) {
        c = std::tolower(c);
    }
    return ext;
}

enum class EncodingCase { UNALIGNED = 0, ALIGNED_WITH_REF = 1, ALIGNED_NO_REF = 2 };

enum class OperationCase { ENCODE = 0, DECODE = 1, CONVERT = 2, CAPSULATE = 3 };

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

        for(auto& i : importers) {
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

        for(auto& r : readCoders) {
            r->setQVCoder(&qvSelector);
        }
    }

    void setNameSelector(std::function<size_t(const genie::core::record::Chunk&)> fun) {
        nameSelector.setSelection(std::move(fun));

        for(auto& r : readCoders) {
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

std::unique_ptr<FlowGraphEncode> buildDefaultEncoder(size_t threads, const std::string& working_dir, size_t au_size) {
    std::unique_ptr<FlowGraphEncode> ret = genie::util::make_unique<FlowGraphEncode>(threads);

    ret->setClassifier(genie::util::make_unique<genie::core::ClassifierBypass>());

    ret->addReadCoder(genie::util::make_unique<genie::read::spring::SpringEncoder>(working_dir, nullptr));
    ret->addReadCoder(genie::util::make_unique<genie::read::localassembly::Encoder>(2048, false));
    ret->addReadCoder(genie::util::make_unique<genie::read::refcoder::Encoder>());
    ret->setReadCoderSelector([](const genie::core::record::Chunk& ) -> size_t { return 1; });

    ret->addQVCoder(genie::util::make_unique<genie::quality::qvwriteout::Encoder>());
    ret->setQVSelector([](const genie::core::record::Chunk& ) -> size_t { return 0; });

    ret->addNameCoder(genie::util::make_unique<genie::core::NameEncoderNone>());
    ret->setNameSelector([](const genie::core::record::Chunk& ) -> size_t { return 0; });

    ret->addEntropyCoder(genie::util::make_unique<genie::entropy::gabac::GabacCompressor>());
    ret->setEntropyCoderSelector([](const genie::core::AccessUnitRaw& ) -> size_t { return 0; });

    ret->setExporterSelector([](const genie::core::AccessUnitPayload& ) -> size_t { return 0; });

    return ret;
}

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    constexpr size_t BLOCKSIZE = 10000;
    auto flow = buildDefaultEncoder(pOpts.numberOfThreads, pOpts.workingDirectory, BLOCKSIZE);

    std::ifstream input_file(pOpts.inputFile);
    flow->addImporter(genie::util::make_unique<genie::format::sam::Importer>(BLOCKSIZE, input_file));

    std::ofstream output_file(pOpts.outputFile);
    flow->addExporter(genie::util::make_unique<genie::format::mgb::Exporter>(&output_file));

    flow->run();

    return 0;
}

}  // namespace run
}  // namespace genieapp
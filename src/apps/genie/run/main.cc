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
#include <genie/util/thread-manager.h>
#include <atomic>
#include <fstream>
#include <functional>
#include <map>
#include <thread>
#include <utility>
#include "program-options.h"
#include <genie/util/selector.h>
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
};

class Preprocessor : public genie::util::Drain<genie::core::record::Chunk>,
                     public genie::util::Source<genie::core::record::Chunk> {
   private:
   public:
};

class FlowGraphEncode : public FlowGraph {
    genie::util::ThreadManager mgr;
    std::vector<std::unique_ptr<genie::core::FormatImporter>> importers;

    std::vector<std::unique_ptr<Preprocessor>> preprocessors;

    std::vector<std::unique_ptr<genie::core::ReadEncoder>> readCoders;
    genie::util::Selector<genie::core::record::Chunk, genie::core::AccessUnitRaw> readSelector;

    std::vector<std::unique_ptr<genie::core::QVEncoder>> qvCoders;
    genie::util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded, const genie::core::record::Chunk&> qvSelector;

    std::vector<std::unique_ptr<genie::core::NameEncoder>> nameCoders;
    genie::util::SideSelector<genie::core::NameEncoder, genie::core::AccessUnitRaw::Descriptor, const genie::core::record::Chunk&>
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
        importers.emplace_back(std::move(dat));
        if(!preprocessors.empty()) {
            importers.back()->setDrain(preprocessors.front().get());
        } else {
            importers.back()->setDrain(&readSelector);
        }
    }

    void addPreprocessor(std::unique_ptr<Preprocessor> dat) {
        if (!preprocessors.empty()) {
            preprocessors.back()->setDrain(dat.get());
        } else {
            for(const auto& imp : importers) {
                imp->setDrain(dat.get());
            }
        }
        preprocessors.emplace_back(std::move(dat));
        preprocessors.back()->setDrain(&readSelector);
    }

    void addReadCoder(std::unique_ptr<genie::core::ReadEncoder> dat) {
        readCoders.emplace_back(std::move(dat));
        readSelector.addBranch(readCoders.back().get(), readCoders.back().get());
    }

    void setReadCoderSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun) {
        readSelector.setOperation(fun);
    }

    void addQVCoder(std::unique_ptr<genie::core::QVEncoder> dat) {
        qvCoders.emplace_back(std::move(dat));
        qvSelector.addMod(qvCoders.back().get());
    }
    void setQVSelector(std::function<size_t(const genie::core::record::Chunk&)> fun) {
        qvSelector.setSelection(std::move(fun));
    }

    void addNameCoder(std::unique_ptr<genie::core::NameEncoder> dat) {
        nameCoders.emplace_back(std::move(dat));
        nameSelector.addMod(nameCoders.back().get());
    }
    void setNameSelector(std::function<size_t(const genie::core::record::Chunk&)> fun) {
        nameSelector.setSelection(std::move(fun));
    }

    void addEntropyCoder(std::unique_ptr<genie::core::EntropyEncoder> dat) {
        entropyCoders.emplace_back(std::move(dat));
        entropySelector.addBranch(entropyCoders.back().get(), entropyCoders.back().get());
    }

    void setEntropyCoderSelector(const std::function<size_t(const genie::core::AccessUnitRaw&)>& fun) {
        entropySelector.setOperation(fun);
    }

    void addExporter(std::unique_ptr<genie::core::FormatExporterCompressed> dat) {
        exporters.emplace_back(std::move(dat));
        exporterSelector.add(exporters.back().get());
    }

    void setExporterSelector(const std::function<size_t(const genie::core::AccessUnitRaw&)>& fun) {
        entropySelector.setOperation(fun);
    }
};

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    constexpr size_t BLOCKSIZE = 10000;
    FlowGraphEncode enc(pOpts.numberOfThreads);

    std::ifstream input_file(pOpts.inputFile);
    enc.addImporter(genie::util::make_unique<genie::format::sam::Importer>(BLOCKSIZE, input_file));

    enc.addReadCoder(genie::util::make_unique<genie::read::spring::SpringEncoder>(pOpts.workingDirectory, nullptr));
    enc.addReadCoder(genie::util::make_unique<genie::read::localassembly::Encoder>(2048, false));
    enc.addReadCoder(genie::util::make_unique<genie::read::refcoder::Encoder>());
    enc.setReadCoderSelector([](const genie::core::record::Chunk& ch)->size_t {
        if(ch.front().getClassID() == genie::core::record::ClassType::CLASS_U) {
            return 0;
        }
        return 1;
    });

    enc.addQVCoder(genie::util::make_unique<genie::quality::qvwriteout::Encoder>());

    enc.addNameCoder(genie::util::make_unique<genie::name::tokenizer::Encoder>());

    enc.addEntropyCoder(genie::util::make_unique<genie::entropy::gabac::GabacCompressor>());

    std::ofstream output_file(pOpts.outputFile);
    enc.addExporter(genie::util::make_unique<genie::format::mgb::Exporter>(&output_file));

    enc.run();

    UTILS_DIE("Run");
    return 0;
}

}  // namespace run
}  // namespace genieapp
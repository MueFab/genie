/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FLOWGRAPH_ENCODE_H
#define GENIE_FLOWGRAPH_ENCODE_H

// ---------------------------------------------------------------------------------------------------------------------

#include "entropy-encoder.h"
#include "flowgraph.h"
#include "format-exporter-compressed.h"
#include "format-importer.h"
#include "read-encoder.h"
#include "reference-source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class FlowGraphEncode : public FlowGraph {
    genie::util::ThreadManager mgr;                       //!<
    std::unique_ptr<genie::core::ReferenceManager> refMgr;
    std::vector<std::unique_ptr<genie::core::ReferenceSource>> refSources;
    std::unique_ptr<genie::core::Classifier> classifier;  //!<

    std::vector<std::unique_ptr<genie::core::FormatImporter>> importers;  //!<

    std::vector<std::unique_ptr<genie::core::ReadEncoder>> readCoders;                        //!<
    genie::util::Selector<genie::core::record::Chunk, genie::core::AccessUnit> readSelector;  //!<

    std::vector<std::unique_ptr<genie::core::QVEncoder>> qvCoders;  //!<
    genie::util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded,
                              const genie::core::record::Chunk&>
        qvSelector;  //!<

    std::vector<std::unique_ptr<genie::core::NameEncoder>> nameCoders;  //!<
    genie::core::ReadEncoder::NameSelector nameSelector;                //!<

    std::vector<std::unique_ptr<genie::core::EntropyEncoder>> entropyCoders;  //!<
    genie::util::SideSelector<genie::core::EntropyEncoder, genie::core::EntropyEncoder::EntropyCoded,
                              genie::core::AccessUnit::Descriptor&>
        entropySelector;  //!<

    std::vector<std::unique_ptr<genie::core::FormatExporterCompressed>> exporters;  //!<
    genie::util::SelectorHead<genie::core::AccessUnit> exporterSelector;            //!<

   public:
    /**
     *
     * @param threads
     */
    explicit FlowGraphEncode(size_t threads);

    /**
     *
     * @param dat
     */
    void addImporter(std::unique_ptr<genie::core::FormatImporter> dat);

    void addReferenceSource(std::unique_ptr<genie::core::ReferenceSource> dat) {
        refSources.push_back(std::move(dat));
        refMgr->addReferenceSource(*refSources.back());
    }

    ReferenceManager& getRefMgr() {
        return *refMgr;
    }

    /**
     *
     * @param _classifier
     */
    void setClassifier(std::unique_ptr<genie::core::Classifier> _classifier);

    /**
     *
     * @param dat
     * @param index
     */
    void setImporter(std::unique_ptr<genie::core::FormatImporter> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addReadCoder(std::unique_ptr<genie::core::ReadEncoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setReadCoder(std::unique_ptr<genie::core::ReadEncoder> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addEntropyCoder(std::unique_ptr<genie::core::EntropyEncoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setEntropyCoder(std::unique_ptr<genie::core::EntropyEncoder> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addExporter(std::unique_ptr<genie::core::FormatExporterCompressed> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setExporter(std::unique_ptr<genie::core::FormatExporterCompressed> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addNameCoder(std::unique_ptr<genie::core::NameEncoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setNameCoder(std::unique_ptr<genie::core::NameEncoder> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addQVCoder(std::unique_ptr<genie::core::QVEncoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setQVCoder(std::unique_ptr<genie::core::QVEncoder> dat, size_t index);

    /**
     *
     * @param fun
     */
    void setReadCoderSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun);

    /**
     *
     * @param fun
     */
    void setQVSelector(std::function<size_t(const genie::core::record::Chunk&)> fun);

    /**
     *
     * @param fun
     */
    void setNameSelector(std::function<size_t(const genie::core::record::Chunk&)> fun);

    /**
     *
     * @param fun
     */
    void setEntropyCoderSelector(const std::function<size_t(const genie::core::AccessUnit::Descriptor&)>& fun);

    /**
     *
     * @param fun
     */
    void setExporterSelector(const std::function<size_t(const genie::core::AccessUnit&)>& fun);

    /**
     *
     */
    void run() override;

    /**
     *
     * @param abort
     */
    void stop(bool abort) override;

    /**
     *
     * @return
     */
    core::stats::PerfStats getStats() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FLOWGRAPH_ENCODE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
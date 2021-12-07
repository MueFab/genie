/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FLOWGRAPH_ENCODE_H_
#define SRC_GENIE_CORE_FLOWGRAPH_ENCODE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/entropy-encoder.h"
#include "genie/core/flowgraph.h"
#include "genie/core/format-exporter-compressed.h"
#include "genie/core/format-importer.h"
#include "genie/core/read-encoder.h"
#include "genie/core/reference-source.h"
#include "genie/util/thread-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class FlowGraphEncode : public FlowGraph {
    genie::util::ThreadManager mgr;                                         //!< @brief
    std::unique_ptr<genie::core::ReferenceManager> refMgr;                  //!< @brief
    std::vector<std::unique_ptr<genie::core::ReferenceSource>> refSources;  //!< @brief
    std::unique_ptr<genie::core::Classifier> classifier;                    //!< @brief

    std::vector<std::unique_ptr<genie::core::FormatImporter>> importers;  //!< @brief

    std::vector<std::unique_ptr<genie::core::ReadEncoder>> readCoders;                        //!< @brief
    genie::util::Selector<genie::core::record::Chunk, genie::core::AccessUnit> readSelector;  //!< @brief

    std::vector<std::unique_ptr<genie::core::QVEncoder>> qvCoders;  //!< @brief
    genie::util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded,
                              const genie::core::record::Chunk&>
        qvSelector;  //!< @brief

    std::vector<std::unique_ptr<genie::core::NameEncoder>> nameCoders;  //!< @brief
    genie::core::ReadEncoder::NameSelector nameSelector;                //!< @brief

    std::vector<std::unique_ptr<genie::core::EntropyEncoder>> entropyCoders;  //!< @brief
    genie::util::SideSelector<genie::core::EntropyEncoder, genie::core::EntropyEncoder::EntropyCoded,
                              genie::core::AccessUnit::Descriptor&>
        entropySelector;  //!< @brief

    std::vector<std::unique_ptr<genie::core::FormatExporterCompressed>> exporters;  //!< @brief
    genie::util::SelectorHead<genie::core::AccessUnit> exporterSelector;            //!< @brief

 public:
    /**
     * @brief
     * @param threads
     */
    explicit FlowGraphEncode(size_t threads);

    /**
     * @brief
     * @param dat
     */
    void addImporter(std::unique_ptr<genie::core::FormatImporter> dat);

    /**
     * @brief
     * @param dat
     */
    void addReferenceSource(std::unique_ptr<genie::core::ReferenceSource> dat);

    /**
     * @brief
     * @return
     */
    ReferenceManager& getRefMgr();

    /**
     * @brief
     * @param _classifier
     */
    void setClassifier(std::unique_ptr<genie::core::Classifier> _classifier);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setImporter(std::unique_ptr<genie::core::FormatImporter> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addReadCoder(std::unique_ptr<genie::core::ReadEncoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setReadCoder(std::unique_ptr<genie::core::ReadEncoder> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addEntropyCoder(std::unique_ptr<genie::core::EntropyEncoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setEntropyCoder(std::unique_ptr<genie::core::EntropyEncoder> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addExporter(std::unique_ptr<genie::core::FormatExporterCompressed> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setExporter(std::unique_ptr<genie::core::FormatExporterCompressed> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addNameCoder(std::unique_ptr<genie::core::NameEncoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setNameCoder(std::unique_ptr<genie::core::NameEncoder> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addQVCoder(std::unique_ptr<genie::core::QVEncoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setQVCoder(std::unique_ptr<genie::core::QVEncoder> dat, size_t index);

    /**
     * @brief
     * @param fun
     */
    void setReadCoderSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun);

    /**
     * @brief
     * @param fun
     */
    void setQVSelector(std::function<size_t(const genie::core::record::Chunk&)> fun);

    /**
     * @brief
     * @param fun
     */
    void setNameSelector(std::function<size_t(const genie::core::record::Chunk&)> fun);

    /**
     * @brief
     * @param fun
     */
    void setEntropyCoderSelector(const std::function<size_t(const genie::core::AccessUnit::Descriptor&)>& fun);

    /**
     * @brief
     * @param fun
     */
    void setExporterSelector(const std::function<size_t(const genie::core::AccessUnit&)>& fun);

    /**
     * @brief
     */
    void run() override;

    /**
     * @brief
     * @param abort
     */
    void stop(bool abort) override;

    /**
     * @brief
     * @return
     */
    core::stats::PerfStats getStats() override;

    /**
     * @brief
     * @return
     */
    core::meta::Dataset getMeta() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FLOWGRAPH_ENCODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

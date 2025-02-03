/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FLOWGRAPH_DECODE_H_
#define SRC_GENIE_CORE_FLOWGRAPH_DECODE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/entropy-decoder.h"
#include "genie/core/flowgraph.h"
#include "genie/core/format-exporter.h"
#include "genie/core/format-importer-compressed.h"
#include "genie/core/read-decoder.h"
#include "genie/core/ref-decoder.h"
#include "genie/core/reference-source.h"
#include "genie/util/selector.h"
#include "genie/util/side_selector.h"
#include "genie/util/thread_manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class FlowGraphDecode : public FlowGraph {
    genie::util::ThreadManager mgr;                                                 //!< @brief
    std::vector<std::unique_ptr<genie::core::FormatImporterCompressed>> importers;  //!< @brief

    std::unique_ptr<genie::core::ReferenceManager> refMgr;                  //!< @brief
    std::vector<std::unique_ptr<genie::core::ReferenceSource>> refSources;  //!< @brief
    genie::core::RefDecoder* refDecoder{nullptr};                           //!< @brief

    std::vector<std::unique_ptr<genie::core::ReadDecoder>> readCoders;                        //!< @brief
    genie::util::Selector<genie::core::AccessUnit, genie::core::record::Chunk> readSelector;  //!< @brief

    std::vector<std::unique_ptr<genie::core::QVDecoder>> qvCoders;  //!< @brief
    genie::core::ReadDecoder::QvSelector qvSelector;                //!< @brief

    std::vector<std::unique_ptr<genie::core::NameDecoder>> nameCoders;  //!< @brief
    core::ReadDecoder::NameSelector nameSelector;                       //!< @brief

    std::vector<std::unique_ptr<genie::core::EntropyDecoder>> entropyCoders;  //!< @brief
    genie::core::ReadDecoder::EntropySelector entropySelector;                //!< @brief

    std::vector<std::unique_ptr<genie::core::FormatExporter>> exporters;     //!< @brief
    genie::util::SelectorHead<genie::core::record::Chunk> exporterSelector;  //!< @brief

 public:
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
     * @param _refDecoder
     */
    void setRefDecoder(genie::core::RefDecoder* _refDecoder);

    /**
     * @brief
     * @return
     */
    genie::core::RefDecoder* getRefDecoder();

    /**
     * @brief
     * @param threads
     */
    explicit FlowGraphDecode(size_t threads);

    /**
     * @brief
     * @param dat
     */
    void addImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addExporter(std::unique_ptr<genie::core::FormatExporter> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setExporter(std::unique_ptr<genie::core::FormatExporter> dat, size_t index);

    /**
     * @brief
     * @param fun
     */
    void setReadCoderSelector(const std::function<size_t(const genie::core::AccessUnit&)>& fun);

    /**
     * @brief
     * @param fun
     */
    void setQVSelector(
        std::function<size_t(const genie::core::parameter::QualityValues& param, const std::vector<std::string>& ecigar,
                             const std::vector<uint64_t>& positions, genie::core::AccessUnit::Descriptor& desc)>
            fun);

    /**
     * @brief
     * @param fun
     */
    void setNameSelector(std::function<size_t(genie::core::AccessUnit::Descriptor& desc)> fun);

    /**
     * @brief
     * @param fun
     */
    void setEntropyCoderSelector(const std::function<size_t(const parameter::DescriptorSubseqCfg&,
                                                            genie::core::AccessUnit::Descriptor&, bool)>& fun);

    /**
     * @brief
     * @param fun
     */
    void setExporterSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun);

    /**
     * @brief
     * @param dat
     */
    void addNameCoder(std::unique_ptr<genie::core::NameDecoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setNameCoder(std::unique_ptr<genie::core::NameDecoder> dat, size_t index);

    /**
     * @brief
     * @param dat
     */
    void addQVCoder(std::unique_ptr<genie::core::QVDecoder> dat);

    /**
     * @brief
     * @param dat
     * @param index
     */
    void setQVCoder(std::unique_ptr<genie::core::QVDecoder> dat, size_t index);

    /**
     * @brief
     */
    void run() override;

    /**
     * @brief
     * @return
     */
    core::stats::PerfStats getStats() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FLOWGRAPH_DECODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

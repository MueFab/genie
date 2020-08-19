/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FLOWGRAPH_DECODE_H
#define GENIE_FLOWGRAPH_DECODE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/ref-decoder.h>
#include <genie/util/selector.h>
#include <genie/util/side-selector.h>
#include <genie/util/thread-manager.h>

#include "entropy-decoder.h"
#include "flowgraph.h"
#include "format-exporter.h"
#include "format-importer-compressed.h"
#include "read-decoder.h"
#include "reference-source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class FlowGraphDecode : public FlowGraph {
    genie::util::ThreadManager mgr;                                                 //!<
    std::vector<std::unique_ptr<genie::core::FormatImporterCompressed>> importers;  //!<

    std::unique_ptr<genie::core::ReferenceManager> refMgr;                  //!<
    std::vector<std::unique_ptr<genie::core::ReferenceSource>> refSources;  //!<
    genie::core::RefDecoder* refDecoder{nullptr};                           //!<

    std::vector<std::unique_ptr<genie::core::ReadDecoder>> readCoders;                        //!<
    genie::util::Selector<genie::core::AccessUnit, genie::core::record::Chunk> readSelector;  //!<

    std::vector<std::unique_ptr<genie::core::QVDecoder>> qvCoders;  //!<
    genie::core::ReadDecoder::QvSelector qvSelector;                //!<

    std::vector<std::unique_ptr<genie::core::NameDecoder>> nameCoders;  //!<
    core::ReadDecoder::NameSelector nameSelector;                       //!<

    std::vector<std::unique_ptr<genie::core::EntropyDecoder>> entropyCoders;  //!<
    genie::core::ReadDecoder::EntropySelector entropySelector;                //!<

    std::vector<std::unique_ptr<genie::core::FormatExporter>> exporters;     //!<
    genie::util::SelectorHead<genie::core::record::Chunk> exporterSelector;  //!<

   public:
    /**
     *
     * @param dat
     */
    void addReferenceSource(std::unique_ptr<genie::core::ReferenceSource> dat);

    /**
     *
     * @return
     */
    ReferenceManager& getRefMgr();

    /**
     *
     * @param _refDecoder
     */
    void setRefDecoder(genie::core::RefDecoder* _refDecoder);

    /**
     *
     * @return
     */
    genie::core::RefDecoder* getRefDecoder();

    /**
     *
     * @param threads
     */
    explicit FlowGraphDecode(size_t threads);

    /**
     *
     * @param dat
     */
    void addImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addExporter(std::unique_ptr<genie::core::FormatExporter> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setExporter(std::unique_ptr<genie::core::FormatExporter> dat, size_t index);

    /**
     *
     * @param fun
     */
    void setReadCoderSelector(const std::function<size_t(const genie::core::AccessUnit&)>& fun);

    /**
     *
     * @param fun
     */
    void setQVSelector(
        std::function<size_t(const genie::core::parameter::QualityValues& param, const std::vector<std::string>& ecigar,
                             genie::core::AccessUnit::Descriptor& desc)>
            fun);

    /**
     *
     * @param fun
     */
    void setNameSelector(std::function<size_t(genie::core::AccessUnit::Descriptor& desc)> fun);

    /**
     *
     * @param fun
     */
    void setEntropyCoderSelector(
        const std::function<size_t(const parameter::DescriptorSubseqCfg&, genie::core::AccessUnit::Descriptor&, bool)>& fun);

    /**
     *
     * @param fun
     */
    void setExporterSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun);

    /**
     *
     * @param dat
     */
    void addNameCoder(std::unique_ptr<genie::core::NameDecoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setNameCoder(std::unique_ptr<genie::core::NameDecoder> dat, size_t index);

    /**
     *
     * @param dat
     */
    void addQVCoder(std::unique_ptr<genie::core::QVDecoder> dat);

    /**
     *
     * @param dat
     * @param index
     */
    void setQVCoder(std::unique_ptr<genie::core::QVDecoder> dat, size_t index);

    /**
     *
     */
    void run() override;

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

#endif  // GENIE_FLOWGRAPH_DECODE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
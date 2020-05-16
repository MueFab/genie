/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FLOWGRAPH_DECODE_H
#define GENIE_FLOWGRAPH_DECODE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/selector.h>
#include <genie/util/sideSelector.h>
#include "entropy-decoder.h"
#include "flowgraph.h"
#include "format-exporter.h"
#include "format-importer-compressed.h"
#include "read-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class FlowGraphDecode : public FlowGraph {
    genie::util::ThreadManager mgr;                                                 //!<
    std::vector<std::unique_ptr<genie::core::FormatImporterCompressed>> importers;  //!<

    std::vector<std::unique_ptr<genie::core::EntropyDecoder>> entropyCoders;                            //!<
    genie::util::Selector<genie::core::AccessUnitPayload, genie::core::AccessUnitRaw> entropySelector;  //!<

    std::vector<std::unique_ptr<genie::core::ReadDecoder>> readCoders;                           //!<
    genie::util::Selector<genie::core::AccessUnitRaw, genie::core::record::Chunk> readSelector;  //!<

    std::vector<std::unique_ptr<genie::core::QVDecoder>> qvCoders;  //!<
    genie::util::SideSelector<genie::core::QVDecoder, std::vector<std::string>,
                              const genie::core::parameter::QualityValues&, const std::vector<std::string>&,
                              genie::core::AccessUnitRaw::Descriptor&>
        qvSelector;  //!<

    std::vector<std::unique_ptr<genie::core::NameDecoder>> nameCoders;  //!<
    genie::util::SideSelector<genie::core::NameDecoder, std::vector<std::string>,
                              genie::core::AccessUnitRaw::Descriptor&>
        nameSelector;  //!<

    std::vector<std::unique_ptr<genie::core::FormatExporter>> exporters;     //!<
    genie::util::SelectorHead<genie::core::record::Chunk> exporterSelector;  //!<

   public:
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
    void setReadCoderSelector(const std::function<size_t(const genie::core::AccessUnitRaw&)>& fun);

    /**
     *
     * @param fun
     */
    void setQVSelector(
        std::function<size_t(const genie::core::parameter::QualityValues& param, const std::vector<std::string>& ecigar,
                             genie::core::AccessUnitRaw::Descriptor& desc)>
            fun);

    /**
     *
     * @param fun
     */
    void setNameSelector(std::function<size_t(genie::core::AccessUnitRaw::Descriptor& desc)> fun);

    /**
     *
     * @param fun
     */
    void setEntropyCoderSelector(const std::function<size_t(const genie::core::AccessUnitPayload&)>& fun);

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
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FLOWGRAPH_DECODE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
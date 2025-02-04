/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/flowgraph_decode.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::addReferenceSource(std::unique_ptr<genie::core::ReferenceSource> dat) {
    refSources.push_back(std::move(dat));
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager& FlowGraphDecode::getRefMgr() { return *refMgr; }

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setRefDecoder(genie::core::RefDecoder* _refDecoder) { refDecoder = _refDecoder; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::RefDecoder* FlowGraphDecode::getRefDecoder() { return refDecoder; }

// ---------------------------------------------------------------------------------------------------------------------

FlowGraphDecode::FlowGraphDecode(size_t threads) : mgr(threads) {
    readSelector.SetDrain(&exporterSelector);
    refMgr = genie::util::make_unique<ReferenceManager>(16);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::addImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat) {
    importers.emplace_back();
    setImporter(std::move(dat), importers.size() - 1);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setImporter(std::unique_ptr<genie::core::FormatImporterCompressed> dat, size_t index) {
    importers[index] = std::move(dat);
    importers[index]->SetDrain(&readSelector);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::addReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat) {
    readCoders.emplace_back(std::move(dat));
    readSelector.AddBranch(readCoders.back().get(), readCoders.back().get());
    readCoders.back()->setQVCoder(&qvSelector);
    readCoders.back()->setNameCoder(&nameSelector);
    readCoders.back()->setEntropyCoder(&entropySelector);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setReadCoder(std::unique_ptr<genie::core::ReadDecoder> dat, size_t index) {
    readCoders[index] = std::move(dat);
    readSelector.SetBranch(readCoders[index].get(), readCoders[index].get(), index);
    readCoders[index]->setQVCoder(&qvSelector);
    readCoders[index]->setNameCoder(&nameSelector);
    readCoders.back()->setEntropyCoder(&entropySelector);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::addEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat) {
    entropyCoders.emplace_back(std::move(dat));
    entropySelector.AddMod(entropyCoders.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setEntropyCoder(std::unique_ptr<genie::core::EntropyDecoder> dat, size_t index) {
    entropyCoders[index] = std::move(dat);
    entropySelector.SetMod(entropyCoders[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::addExporter(std::unique_ptr<genie::core::FormatExporter> dat) {
    exporters.emplace_back(std::move(dat));
    exporterSelector.Add(exporters.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setExporter(std::unique_ptr<genie::core::FormatExporter> dat, size_t index) {
    exporters[index] = std::move(dat);
    exporterSelector.Set(exporters[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setReadCoderSelector(const std::function<size_t(const genie::core::AccessUnit&)>& fun) {
    readSelector.SetOperation(fun);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setQVSelector(
    std::function<size_t(const genie::core::parameter::QualityValues& param, const std::vector<std::string>& ecigar,
                         const std::vector<uint64_t>& positions, genie::core::AccessUnit::Descriptor& desc)>
        fun) {
    qvSelector.SetSelection(std::move(fun));

    for (auto& r : readCoders) {
        r->setQVCoder(&qvSelector);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setNameSelector(std::function<size_t(genie::core::AccessUnit::Descriptor& desc)> fun) {
    nameSelector.SetSelection(std::move(fun));

    for (auto& r : readCoders) {
        r->setNameCoder(&nameSelector);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setEntropyCoderSelector(
    const std::function<size_t(const parameter::DescriptorSubseqCfg&, genie::core::AccessUnit::Descriptor&, bool)>&
        fun) {
    entropySelector.SetSelection(fun);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setExporterSelector(const std::function<size_t(const genie::core::record::Chunk&)>& fun) {
    exporterSelector.SetOperation(fun);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::addNameCoder(std::unique_ptr<genie::core::NameDecoder> dat) {
    nameCoders.emplace_back(std::move(dat));
    nameSelector.AddMod(nameCoders.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setNameCoder(std::unique_ptr<genie::core::NameDecoder> dat, size_t index) {
    nameCoders[index] = std::move(dat);
    nameSelector.SetMod(nameCoders[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::addQVCoder(std::unique_ptr<genie::core::QVDecoder> dat) {
    qvCoders.emplace_back(std::move(dat));
    qvSelector.AddMod(qvCoders.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::setQVCoder(std::unique_ptr<genie::core::QVDecoder> dat, size_t index) {
    qvCoders[index] = std::move(dat);
    qvSelector.SetMod(qvCoders[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphDecode::run() {
    std::vector<genie::util::OriginalSource*> imps;
    for (auto& i : importers) {
        imps.emplace_back(i.get());
    }
    mgr.SetSource(std::move(imps));
    mgr.Run();
}

// ---------------------------------------------------------------------------------------------------------------------

core::stats::PerfStats FlowGraphDecode::getStats() {
    core::stats::PerfStats ret;
    for (auto& e : exporters) {
        ret.add(e->getStats());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

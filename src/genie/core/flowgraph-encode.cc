/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/flowgraph-encode.h"
#include <memory>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::addReferenceSource(std::unique_ptr<ReferenceSource> dat) {
    refSources.push_back(std::move(dat));
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager& FlowGraphEncode::getRefMgr() { return *refMgr; }

// ---------------------------------------------------------------------------------------------------------------------

FlowGraphEncode::FlowGraphEncode(size_t threads) : mgr(threads) {
    readSelector.setDrain(&exporterSelector);
    refMgr = std::make_unique<ReferenceManager>(16);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::addImporter(std::unique_ptr<FormatImporter> dat) {
    importers.emplace_back();
    setImporter(std::move(dat), importers.size() - 1);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setClassifier(std::unique_ptr<Classifier> _classifier) {
    classifier = std::move(_classifier);

    for (auto& i : importers) {
        i->setClassifier(classifier.get());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setImporter(std::unique_ptr<FormatImporter> dat, size_t index) {
    importers[index] = std::move(dat);
    importers[index]->setDrain(&readSelector);
    importers[index]->setClassifier(classifier.get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::addReadCoder(std::unique_ptr<ReadEncoder> dat) {
    readCoders.emplace_back(std::move(dat));
    readSelector.addBranch(readCoders.back().get(), readCoders.back().get());
    readCoders.back()->setQVCoder(&qvSelector);
    readCoders.back()->setNameCoder(&nameSelector);
    readCoders.back()->setEntropyCoder(&entropySelector);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setReadCoder(std::unique_ptr<ReadEncoder> dat, size_t index) {
    readCoders[index] = std::move(dat);
    readSelector.setBranch(readCoders[index].get(), readCoders[index].get(), index);
    readCoders[index]->setQVCoder(&qvSelector);
    readCoders[index]->setNameCoder(&nameSelector);
    readCoders[index]->setEntropyCoder(&entropySelector);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::addEntropyCoder(std::unique_ptr<EntropyEncoder> dat) {
    entropyCoders.emplace_back(std::move(dat));
    entropySelector.addMod(entropyCoders.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setEntropyCoder(std::unique_ptr<EntropyEncoder> dat, size_t index) {
    entropyCoders[index] = std::move(dat);
    entropySelector.setMod(entropyCoders[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::addExporter(std::unique_ptr<FormatExporterCompressed> dat) {
    exporters.emplace_back(std::move(dat));
    exporterSelector.add(exporters.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setExporter(std::unique_ptr<FormatExporterCompressed> dat, size_t index) {
    exporters[index] = std::move(dat);
    exporterSelector.set(exporters[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::addNameCoder(std::unique_ptr<NameEncoder> dat) {
    nameCoders.emplace_back(std::move(dat));
    nameSelector.addMod(nameCoders.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setNameCoder(std::unique_ptr<NameEncoder> dat, size_t index) {
    nameCoders[index] = std::move(dat);
    nameSelector.setMod(nameCoders[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::addQVCoder(std::unique_ptr<QVEncoder> dat) {
    qvCoders.emplace_back(std::move(dat));
    qvSelector.addMod(qvCoders.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setQVCoder(std::unique_ptr<QVEncoder> dat, size_t index) {
    qvCoders[index] = std::move(dat);
    qvSelector.setMod(qvCoders[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setReadCoderSelector(const std::function<size_t(const record::Chunk&)>& fun) {
    readSelector.setOperation(fun);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setQVSelector(std::function<size_t(const record::Chunk&)> fun) {
    qvSelector.setSelection(std::move(fun));

    for (auto& r : readCoders) {
        r->setQVCoder(&qvSelector);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setNameSelector(std::function<size_t(const record::Chunk&)> fun) {
    nameSelector.setSelection(std::move(fun));

    for (auto& r : readCoders) {
        r->setNameCoder(&nameSelector);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setEntropyCoderSelector(
    const std::function<size_t(const AccessUnit::Descriptor&)>& fun) {
    entropySelector.setSelection(fun);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::setExporterSelector(const std::function<size_t(const AccessUnit&)>& fun) {
    exporterSelector.setOperation(fun);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::run() {
    std::vector<util::OriginalSource*> imps;
    imps.reserve(importers.size());
    for (auto& i : importers) {
        imps.emplace_back(i.get());
    }
    mgr.setSource(std::move(imps));
    mgr.run();
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphEncode::stop(bool abort) { mgr.stop(abort); }

// ---------------------------------------------------------------------------------------------------------------------

stats::PerfStats FlowGraphEncode::getStats() {
    stats::PerfStats ret;
    for (auto& e : exporters) {
        ret.add(e->getStats());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

meta::Dataset FlowGraphEncode::getMeta() const {
    meta::Dataset ret = {0, std::make_unique<meta::blockheader::Enabled>(false, false), "", ""};
    if (!refSources.empty()) {
        ret.setReference(this->refSources.front()->getMeta());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

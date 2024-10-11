/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/flowgraph-convert.h"
#include <memory>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

// ---------------------------------------------------------------------------------------------------------------------

FlowGraphConvert::FlowGraphConvert(size_t threads) : mgr(threads) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager& FlowGraphConvert::getRefMgr() { return *refMgr; }

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::addImporter(std::unique_ptr<FormatImporter> dat) {
    importers.emplace_back();
    setImporter(std::move(dat), importers.size() - 1);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::setClassifier(std::unique_ptr<Classifier> _classifier) {
    classifier = std::move(_classifier);

    for (auto& i : importers) {
        i->setClassifier(classifier.get());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::setImporter(std::unique_ptr<FormatImporter> dat, size_t index) {
    importers[index] = std::move(dat);
    importers[index]->setClassifier(classifier.get());
    importers[index]->setDrain(&exporterSelector);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::addExporter(std::unique_ptr<FormatExporter> dat) {
    exporters.emplace_back(std::move(dat));
    exporterSelector.add(exporters.back().get());
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::setExporter(std::unique_ptr<FormatExporter> dat, size_t index) {
    exporters[index] = std::move(dat);
    exporterSelector.set(exporters[index].get(), index);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::setExporterSelector(const std::function<size_t(const record::Chunk&)>& fun) {
    exporterSelector.setOperation(fun);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::run() {
    std::vector<util::OriginalSource*> imps;
    imps.reserve(importers.size());
    for (auto& i : importers) {
        imps.emplace_back(i.get());
    }
    mgr.setSource(std::move(imps));
    mgr.run();
}

// ---------------------------------------------------------------------------------------------------------------------

void FlowGraphConvert::stop(bool abort) { mgr.stop(abort); }

// ---------------------------------------------------------------------------------------------------------------------

stats::PerfStats FlowGraphConvert::getStats() {
    stats::PerfStats ret;
    for (auto& e : exporters) {
        ret.add(e->getStats());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

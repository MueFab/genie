/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/flow_graph_convert.h"

#include <memory>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

FlowGraphConvert::FlowGraphConvert(const size_t threads) : mgr_(threads) {}

// -----------------------------------------------------------------------------

ReferenceManager& FlowGraphConvert::GetRefMgr() const { return *ref_mgr_; }

// -----------------------------------------------------------------------------

void FlowGraphConvert::AddImporter(std::unique_ptr<FormatImporter> dat) {
  importers_.emplace_back();
  SetImporter(std::move(dat), importers_.size() - 1);
}

// -----------------------------------------------------------------------------

void FlowGraphConvert::SetClassifier(std::unique_ptr<Classifier> classifier) {
  classifier_ = std::move(classifier);

  for (const auto& i : importers_) {
    i->SetClassifier(classifier_.get());
  }
}

// -----------------------------------------------------------------------------

void FlowGraphConvert::SetImporter(std::unique_ptr<FormatImporter> dat,
                                   const size_t index) {
  importers_[index] = std::move(dat);
  importers_[index]->SetClassifier(classifier_.get());
  importers_[index]->SetDrain(&exporter_selector_);
}

// -----------------------------------------------------------------------------

void FlowGraphConvert::AddExporter(std::unique_ptr<FormatExporter> dat) {
  exporters_.emplace_back(std::move(dat));
  exporter_selector_.Add(exporters_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphConvert::SetExporter(std::unique_ptr<FormatExporter> dat,
                                   const size_t index) {
  exporters_[index] = std::move(dat);
  exporter_selector_.Set(exporters_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphConvert::SetExporterSelector(
    const std::function<size_t(const record::Chunk&)>& fun) {
  exporter_selector_.SetOperation(fun);
}

// -----------------------------------------------------------------------------

void FlowGraphConvert::Run() {
  std::vector<util::OriginalSource*> imps;
  imps.reserve(importers_.size());
  for (auto& i : importers_) {
    imps.emplace_back(i.get());
  }
  mgr_.SetSource(std::move(imps));
  mgr_.Run();
}

// -----------------------------------------------------------------------------

void FlowGraphConvert::Stop(const bool abort) { mgr_.Stop(abort); }

// -----------------------------------------------------------------------------

stats::PerfStats FlowGraphConvert::GetStats() {
  stats::PerfStats ret;
  for (const auto& e : exporters_) {
    ret.Add(e->GetStats());
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

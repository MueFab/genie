/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/flow_graph_encode.h"

#include <memory>
#include <utility>
#include <vector>

#include "meta/block_header/enabled.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
void FlowGraphEncode::AddReferenceSource(std::unique_ptr<ReferenceSource> dat) {
  ref_sources_.push_back(std::move(dat));
}

// -----------------------------------------------------------------------------
ReferenceManager& FlowGraphEncode::GetRefMgr() const { return *ref_mgr_; }

// -----------------------------------------------------------------------------
FlowGraphEncode::FlowGraphEncode(const size_t threads) : mgr_(threads) {
  read_selector_.SetDrain(&exporter_selector_);
  ref_mgr_ = std::make_unique<ReferenceManager>(16);
}

// -----------------------------------------------------------------------------
void FlowGraphEncode::AddImporter(std::unique_ptr<FormatImporter> dat) {
  importers_.emplace_back();
  SetImporter(std::move(dat), importers_.size() - 1);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetClassifier(std::unique_ptr<Classifier> classifier) {
  classifier_ = std::move(classifier);

  for (const auto& i : importers_) {
    i->SetClassifier(classifier_.get());
  }
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetImporter(std::unique_ptr<FormatImporter> dat,
                                  const size_t index) {
  importers_[index] = std::move(dat);
  importers_[index]->SetDrain(&read_selector_);
  importers_[index]->SetClassifier(classifier_.get());
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::AddReadCoder(std::unique_ptr<ReadEncoder> dat) {
  read_coders_.emplace_back(std::move(dat));
  read_selector_.AddBranch(read_coders_.back().get(),
                           read_coders_.back().get());
  read_coders_.back()->SetQvCoder(&qv_selector_);
  read_coders_.back()->SetNameCoder(&name_selector_);
  read_coders_.back()->SetEntropyCoder(&entropy_selector_);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetReadCoder(std::unique_ptr<ReadEncoder> dat,
                                   const size_t index) {
  read_coders_[index] = std::move(dat);
  read_selector_.SetBranch(read_coders_[index].get(), read_coders_[index].get(),
                           index);
  read_coders_[index]->SetQvCoder(&qv_selector_);
  read_coders_[index]->SetNameCoder(&name_selector_);
  read_coders_[index]->SetEntropyCoder(&entropy_selector_);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::AddEntropyCoder(std::unique_ptr<EntropyEncoder> dat) {
  entropy_coders_.emplace_back(std::move(dat));
  entropy_selector_.AddMod(entropy_coders_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetEntropyCoder(std::unique_ptr<EntropyEncoder> dat,
                                      const size_t index) {
  entropy_coders_[index] = std::move(dat);
  entropy_selector_.SetMod(entropy_coders_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::AddExporter(
    std::unique_ptr<FormatExporterCompressed> dat) {
  exporters_.emplace_back(std::move(dat));
  exporter_selector_.Add(exporters_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetExporter(std::unique_ptr<FormatExporterCompressed> dat,
                                  const size_t index) {
  exporters_[index] = std::move(dat);
  exporter_selector_.Set(exporters_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::AddNameCoder(std::unique_ptr<NameEncoder> dat) {
  name_coders_.emplace_back(std::move(dat));
  name_selector_.AddMod(name_coders_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetNameCoder(std::unique_ptr<NameEncoder> dat,
                                   const size_t index) {
  name_coders_[index] = std::move(dat);
  name_selector_.SetMod(name_coders_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::AddQvCoder(std::unique_ptr<QvEncoder> dat) {
  qv_coders_.emplace_back(std::move(dat));
  qv_selector_.AddMod(qv_coders_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetQvCoder(std::unique_ptr<QvEncoder> dat,
                                 const size_t index) {
  qv_coders_[index] = std::move(dat);
  qv_selector_.SetMod(qv_coders_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetReadCoderSelector(
    const std::function<size_t(const record::Chunk&)>& fun) {
  read_selector_.SetOperation(fun);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetQvSelector(
    std::function<size_t(const record::Chunk&)> fun) {
  qv_selector_.SetSelection(std::move(fun));

  for (const auto& r : read_coders_) {
    r->SetQvCoder(&qv_selector_);
  }
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetNameSelector(
    std::function<size_t(const record::Chunk&)> fun) {
  name_selector_.SetSelection(std::move(fun));

  for (const auto& r : read_coders_) {
    r->SetNameCoder(&name_selector_);
  }
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetEntropyCoderSelector(
    const std::function<size_t(const AccessUnit::Descriptor&)>& fun) {
  entropy_selector_.SetSelection(fun);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::SetExporterSelector(
    const std::function<size_t(const AccessUnit&)>& fun) {
  exporter_selector_.SetOperation(fun);
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::Run() {
  std::vector<util::OriginalSource*> imps;
  imps.reserve(importers_.size());
  for (auto& i : importers_) {
    imps.emplace_back(i.get());
  }
  mgr_.SetSource(std::move(imps));
  mgr_.Run();
}

// -----------------------------------------------------------------------------

void FlowGraphEncode::Stop(const bool abort) { mgr_.Stop(abort); }

// -----------------------------------------------------------------------------

stats::PerfStats FlowGraphEncode::GetStats() {
  stats::PerfStats ret;
  for (const auto& e : exporters_) {
    ret.Add(e->GetStats());
  }
  return ret;
}

// -----------------------------------------------------------------------------

meta::Dataset FlowGraphEncode::GetMeta() const {
  meta::Dataset ret = {
      0, std::make_unique<meta::block_header::Enabled>(false, false), "", ""};
  if (!ref_sources_.empty()) {
    ret.SetReference(this->ref_sources_.front()->GetMeta());
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

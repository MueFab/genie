/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/flow_graph_decode.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
void FlowGraphDecode::AddReferenceSource(std::unique_ptr<ReferenceSource> dat) {
  ref_sources_.push_back(std::move(dat));
}

// -----------------------------------------------------------------------------
ReferenceManager& FlowGraphDecode::GetRefMgr() const { return *ref_mgr_; }

// -----------------------------------------------------------------------------
void FlowGraphDecode::SetRefDecoder(RefDecoder* ref_decoder) {
  ref_decoder_ = ref_decoder;
}

// -----------------------------------------------------------------------------
RefDecoder* FlowGraphDecode::GetRefDecoder() const { return ref_decoder_; }

// -----------------------------------------------------------------------------
FlowGraphDecode::FlowGraphDecode(const size_t threads) : mgr_(threads) {
  read_selector_.SetDrain(&exporter_selector_);
  ref_mgr_ = std::make_unique<ReferenceManager>(16);
}

// -----------------------------------------------------------------------------
void FlowGraphDecode::AddImporter(
    std::unique_ptr<FormatImporterCompressed> dat) {
  importers_.emplace_back();
  SetImporter(std::move(dat), importers_.size() - 1);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetImporter(std::unique_ptr<FormatImporterCompressed> dat,
                                  const size_t index) {
  importers_[index] = std::move(dat);
  importers_[index]->SetDrain(&read_selector_);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::AddReadCoder(std::unique_ptr<ReadDecoder> dat) {
  read_coders_.emplace_back(std::move(dat));
  read_selector_.AddBranch(read_coders_.back().get(),
                           read_coders_.back().get());
  read_coders_.back()->SetQvCoder(&qv_selector_);
  read_coders_.back()->SetNameCoder(&name_selector_);
  read_coders_.back()->SetEntropyCoder(&entropy_selector_);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetReadCoder(std::unique_ptr<ReadDecoder> dat,
                                   const size_t index) {
  read_coders_[index] = std::move(dat);
  read_selector_.SetBranch(read_coders_[index].get(), read_coders_[index].get(),
                           index);
  read_coders_[index]->SetQvCoder(&qv_selector_);
  read_coders_[index]->SetNameCoder(&name_selector_);
  read_coders_.back()->SetEntropyCoder(&entropy_selector_);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::AddEntropyCoder(std::unique_ptr<EntropyDecoder> dat) {
  entropy_coders_.emplace_back(std::move(dat));
  entropy_selector_.AddMod(entropy_coders_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetEntropyCoder(std::unique_ptr<EntropyDecoder> dat,
                                      const size_t index) {
  entropy_coders_[index] = std::move(dat);
  entropy_selector_.SetMod(entropy_coders_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::AddExporter(std::unique_ptr<FormatExporter> dat) {
  exporters_.emplace_back(std::move(dat));
  exporter_selector_.Add(exporters_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetExporter(std::unique_ptr<FormatExporter> dat,
                                  const size_t index) {
  exporters_[index] = std::move(dat);
  exporter_selector_.Set(exporters_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetReadCoderSelector(
    const std::function<size_t(const AccessUnit&)>& fun) {
  read_selector_.SetOperation(fun);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetQvSelector(
    std::function<size_t(const parameter::QualityValues& param,
                         const std::vector<std::string>& e_cigar,
                         const std::vector<uint64_t>& positions,
                         AccessUnit::Descriptor& desc)>
        fun) {
  qv_selector_.SetSelection(std::move(fun));

  for (const auto& r : read_coders_) {
    r->SetQvCoder(&qv_selector_);
  }
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetNameSelector(
    std::function<size_t(AccessUnit::Descriptor& desc)> fun) {
  name_selector_.SetSelection(std::move(fun));

  for (const auto& r : read_coders_) {
    r->SetNameCoder(&name_selector_);
  }
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetEntropyCoderSelector(
    const std::function<size_t(const parameter::DescriptorSubSequenceCfg&,
                               AccessUnit::Descriptor&, bool)>& fun) {
  entropy_selector_.SetSelection(fun);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetExporterSelector(
    const std::function<size_t(const record::Chunk&)>& fun) {
  exporter_selector_.SetOperation(fun);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::AddNameCoder(std::unique_ptr<NameDecoder> dat) {
  name_coders_.emplace_back(std::move(dat));
  name_selector_.AddMod(name_coders_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetNameCoder(std::unique_ptr<NameDecoder> dat,
                                   const size_t index) {
  name_coders_[index] = std::move(dat);
  name_selector_.SetMod(name_coders_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::AddQvCoder(std::unique_ptr<QvDecoder> dat) {
  qv_coders_.emplace_back(std::move(dat));
  qv_selector_.AddMod(qv_coders_.back().get());
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::SetQvCoder(std::unique_ptr<QvDecoder> dat,
                                 const size_t index) {
  qv_coders_[index] = std::move(dat);
  qv_selector_.SetMod(qv_coders_[index].get(), index);
}

// -----------------------------------------------------------------------------

void FlowGraphDecode::Run() {
  std::vector<util::OriginalSource*> imps;
  imps.reserve(importers_.size());
  for (auto& i : importers_) {
    imps.emplace_back(i.get());
  }
  mgr_.SetSource(std::move(imps));
  mgr_.Run();
}

// -----------------------------------------------------------------------------

stats::PerfStats FlowGraphDecode::GetStats() {
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

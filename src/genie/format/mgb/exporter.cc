/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/exporter.h"

#include <iostream>
#include <string>
#include <utility>

#include "genie/format/mgb/raw_reference.h"
#include "genie/util/log.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------


constexpr auto kLogModuleName = "Mgb";

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
Exporter::Exporter(std::ostream* file) : writer(*file), id_ctr(0) {}

// -----------------------------------------------------------------------------
void Exporter::FlowIn(core::AccessUnit&& t, const util::Section& id) {
  util::Watch watch;
  core::AccessUnit data = std::move(t);
  [[maybe_unused]] util::OrderedSection section(&lock, id);
  GetStats().Add(data.GetStats());
  auto parameter_id = static_cast<uint8_t>(parameter_stash.size());
  core::parameter::ParameterSet out_set(parameter_id, parameter_id,
                                        std::move(data.GetParameters()));
  RawReference ref;
  for (const auto& [fst, snd] : data.GetRefToWrite()) {
    auto string = *data.GetReferenceExcerpt().GetChunkAt(fst);
    auto substr = string.substr(fst % core::ReferenceManager::GetChunkSize());
    substr = string.substr(0, snd - fst);
    RawReferenceSequence refseq(data.GetReference(), fst, std::move(substr));
    ref.AddSequence(std::move(refseq));
  }
  if (!ref.IsEmpty()) {
    for (auto& r : ref) {
      GENIE_LOG(util::Logger::Severity::INFO,
                "Writing Ref " + std::to_string(r.GetSeqId()) + ":" +
                    std::to_string(r.GetStart()) + "-" +
                    std::to_string(r.GetEnd()));
    }
    ref.Write(writer);
    ref = RawReference();
  }

  if (data.GetNumReads() == 0) {
    return;
  }

  bool found = false;
  for (const auto& p : parameter_stash) {
    if (out_set == p) {
      found = true;
      parameter_id = p.GetId();
    }
  }

  if (!found) {
    GENIE_LOG(util::Logger::Severity::INFO,
              "Writing parameter set " +
                  std::to_string(static_cast<uint32_t>(out_set.GetId())));
    out_set.Write(writer);
    parameter_stash.push_back(out_set);
  }

  auto dataset_type = data.GetClassType() != core::record::ClassType::kClassU
                         ? core::parameter::DataUnit::DatasetType::kAligned
                     : data.IsReferenceOnly()
                         ? core::parameter::DataUnit::DatasetType::kReference
                         : core::parameter::DataUnit::DatasetType::kNonAligned;

  AccessUnit au(static_cast<uint32_t>(id_ctr), parameter_id,
                data.GetClassType(), static_cast<uint32_t>(data.GetNumReads()),
                dataset_type, 32, false, core::AlphabetId::kAcgtn);
  if (data.IsReferenceOnly()) {
    au.GetHeader().SetRefCfg(
        RefCfg(data.GetReference(), data.GetReferenceExcerpt().GetGlobalStart(),
               data.GetReferenceExcerpt().GetGlobalEnd() - 1, 32));
  }
  if (au.GetHeader().GetClass() != core::record::ClassType::kClassU) {
    au.GetHeader().SetAuTypeCfg(AuTypeCfg(data.GetReference(), data.GetMinPos(),
                                          data.GetMaxPos(),
                                          data.GetParameters().GetPosSize()));
  }
  for (uint8_t descriptor = 0;
       descriptor < static_cast<uint8_t>(core::GetDescriptors().size());
       ++descriptor) {
    if (data.Get(static_cast<core::GenDesc>(descriptor)).IsEmpty()) {
      continue;
    }
    au.AddBlock(
        Block(descriptor,
              std::move(data.Get(static_cast<core::GenDesc>(descriptor)))));
  }

  GENIE_LOG(util::Logger::Severity::INFO, "Writing access unit " +
    au.DebugPrint(
      parameter_stash[au.GetHeader().GetParameterId()].GetEncodingSet()));

  au.Write(writer);
  id_ctr++;
  GetStats().AddDouble("time-mgb-export", watch.Check());
}

// -----------------------------------------------------------------------------
void Exporter::SkipIn(const util::Section& id) {
  [[maybe_unused]] util::OrderedSection sec(&lock, id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

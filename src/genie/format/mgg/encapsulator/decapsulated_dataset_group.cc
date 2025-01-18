/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/encapsulator/decapsulated_dataset_group.h"

#include <map>
#include <memory>
#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

// -----------------------------------------------------------------------------

DecapsulatedDatasetGroup::DecapsulatedDatasetGroup(
    DecapsulatedDatasetGroup&& other) noexcept {
  id = other.id;
  meta_group = std::move(other.meta_group);
  meta_references = std::move(other.meta_references);
  data = std::move(other.data);
}

// -----------------------------------------------------------------------------

DecapsulatedDatasetGroup::DecapsulatedDatasetGroup(DatasetGroup* grp) {
  id = grp->GetHeader().GetId();
  meta_group = decapsulate_dataset_group(grp);
  meta_references = decapsulate_references(grp);

  for (auto& dt : grp->GetDatasets()) {
    data.emplace(dt.GetHeader().GetDatasetId(),
                 decapsulate_dataset(dt, meta_group, meta_references, grp));
  }
}

// -----------------------------------------------------------------------------

uint64_t DecapsulatedDatasetGroup::GetId() const { return id; }

// -----------------------------------------------------------------------------

bool DecapsulatedDatasetGroup::HasMetaGroup() const {
  return meta_group != std::nullopt;
}

// -----------------------------------------------------------------------------

core::meta::DatasetGroup& DecapsulatedDatasetGroup::GetMetaGroup() {
  return *meta_group;
}

// -----------------------------------------------------------------------------

std::map<uint8_t, core::meta::Reference>&
DecapsulatedDatasetGroup::GetMetaReferences() {
  return meta_references;
}

// -----------------------------------------------------------------------------

std::map<uint64_t, std::pair<mgb::MgbFile, core::meta::Dataset>>&
DecapsulatedDatasetGroup::GetData() {
  return data;
}

// -----------------------------------------------------------------------------

std::optional<core::meta::DatasetGroup>
DecapsulatedDatasetGroup::decapsulate_dataset_group(DatasetGroup* grp) {
  std::optional<core::meta::DatasetGroup> meta_group;
  if (grp->HasMetadata()) {
    if (meta_group == std::nullopt) {
      meta_group = core::meta::DatasetGroup(
          grp->GetHeader().GetId(), grp->GetHeader().GetVersion(), "", "");
    }
    meta_group->SetMetadata(grp->GetMetadata().decapsulate());
  }

  if (grp->HasProtection()) {
    if (meta_group == std::nullopt) {
      meta_group = core::meta::DatasetGroup(
          grp->GetHeader().GetId(), grp->GetHeader().GetVersion(), "", "");
    }
    meta_group->SetProtection(grp->GetProtection().decapsulate());
  }
  return meta_group;
}

// -----------------------------------------------------------------------------

std::map<uint8_t, core::meta::Reference>
DecapsulatedDatasetGroup::decapsulate_references(DatasetGroup* grp) {
  std::map<uint8_t, std::string> ref_metadata;

  for (auto& m : grp->GetReferenceMetadata()) {
    ref_metadata.emplace(m.GetReferenceId(), m.decapsulate());
  }

  std::map<uint8_t, core::meta::Reference> references;

  for (auto& m : grp->GetReferences()) {
    auto it = ref_metadata.find(m.GetReferenceId());
    std::string meta;
    if (it != ref_metadata.end()) {
      meta = std::move(it->second);
    }
    references.emplace(m.GetReferenceId(), m.decapsulate(std::move(meta)));
  }
  return references;
}

// -----------------------------------------------------------------------------

std::pair<mgb::AccessUnit, std::optional<core::meta::AccessUnit>>
DecapsulatedDatasetGroup::DecapsulateAu(AccessUnit& au) {
  core::meta::AccessUnit meta_au(au.GetHeader().GetHeader().GetId());
  bool has_meta = false;
  if (au.HasInformation()) {
    has_meta = true;
    meta_au.SetInformation(au.GetInformation().decapsulate());
  }
  if (au.HasProtection()) {
    has_meta = true;
    meta_au.SetProtection(au.GetProtection().decapsulate());
  }
  std::pair<mgb::AccessUnit, std::optional<core::meta::AccessUnit>> ret(
      au.Decapsulate(), std::nullopt);
  if (has_meta) {
    ret.second = std::move(meta_au);
  }
  return ret;
}

// -----------------------------------------------------------------------------

std::pair<mgb::MgbFile, core::meta::Dataset>
DecapsulatedDatasetGroup::decapsulate_dataset(
    Dataset& dt, const std::optional<core::meta::DatasetGroup>& meta_group,
    std::map<uint8_t, core::meta::Reference>& meta_references,
    DatasetGroup* grp) {
  mgb::MgbFile mgb_file;
  core::meta::Dataset meta;

  meta.SetHeader(dt.GetHeader().decapsulate());

  if (!dt.GetHeader().GetReferenceOptions().GetSeqIDs().empty()) {
    if (const auto it = meta_references.find(
            dt.GetHeader().GetReferenceOptions().GetReferenceId());
        it != meta_references.end()) {
      meta.SetReference(it->second);
    }
  }

  if (meta_group != std::nullopt) {
    meta.SetDataGroup(*meta_group);
  }
  if (grp->HasLabelList()) {
    auto labels =
        grp->GetLabelList().Decapsulate(dt.GetHeader().GetDatasetId());
    for (auto& l : labels) {
      meta.AddLabel(std::move(l));
    }
  }
  for (auto& p : dt.GetParameterSets()) {
    mgb_file.AddUnit(
        std::make_unique<core::parameter::ParameterSet>(p.descapsulate()));
  }
  for (auto& au : dt.GetAccessUnits()) {
    auto [fst, snd] = DecapsulateAu(au);
    mgb_file.AddUnit(std::make_unique<mgb::AccessUnit>(std::move(fst)));
    if (snd != std::nullopt) {
      meta.AddAccessUnit(std::move(*snd));
    }
  }

  if (dt.HasMetadata()) {
    meta.SetMetadata(dt.GetMetadata().decapsulate());
  }

  if (dt.HasProtection()) {
    meta.SetProtection(dt.GetProtection().Decapsulate());
  }

  for (auto& ds : dt.GetDescriptorStreams()) {
    if (ds.HasProtection()) {
      meta.AddDescriptorStream(core::meta::DescriptorStream(
          static_cast<size_t>(ds.GetHeader().GetDescriptorId()),
          ds.GetProtection().decapsulate()));
    }
  }

  return std::make_pair(std::move(mgb_file), std::move(meta));
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

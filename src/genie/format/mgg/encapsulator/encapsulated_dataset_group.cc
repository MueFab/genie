/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/encapsulator/encapsulated_dataset_group.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

// -----------------------------------------------------------------------------
void EncapsulatedDatasetGroup::PatchId(const uint8_t id) {
  if (group_meta != std::nullopt) {
    group_meta->PatchId(id);
  }
  if (group_protection != std::nullopt) {
    group_protection->PatchId(id);
  }
  for (auto& r : references) {
    r.PatchId(id);
  }
  for (auto& r : reference_meta) {
    r.PatchId(id);
  }
  for (const auto& r : datasets) {
    for (auto& d : r->datasets) {
      d.PatchId(id, d.GetHeader().GetDatasetId());
    }
  }
  if (label_list != std::nullopt) {
    label_list->PatchId(id);
  }
}

// -----------------------------------------------------------------------------
void EncapsulatedDatasetGroup::MergeMetadata(
    const core::MpegMinorVersion version) {
  std::string meta;
  std::string protection;

  for (const auto& d : datasets) {
    if (d->meta.GetDataGroup() == std::nullopt) {
      continue;
    }
    if (protection != d->meta.GetDataGroup()->GetProtection()) {
      UTILS_DIE_IF(!protection.empty() &&
                       !d->meta.GetDataGroup()->GetProtection().empty(),
                   "Contradicting config");
      if (protection.empty()) {
        protection = std::move(d->meta.GetDataGroup()->GetProtection());
      }
    }
    if (meta != d->meta.GetDataGroup()->GetInformation()) {
      UTILS_DIE_IF(
          !meta.empty() && !d->meta.GetDataGroup()->GetInformation().empty(),
          "Contradicting config");
      if (meta.empty()) {
        meta = std::move(d->meta.GetDataGroup()->GetInformation());
      }
    }
  }

  if (!meta.empty()) {
    group_meta = DatasetGroupMetadata(0, std::move(meta), version);
  }

  if (!protection.empty()) {
    group_protection =
        DatasetGroupProtection(0, std::move(protection), version);
  }
}

// -----------------------------------------------------------------------------
void EncapsulatedDatasetGroup::MergeReferences(
    const core::MpegMinorVersion version) {
  for (const auto& d : datasets) {
    if (d->meta.GetReference() == std::nullopt) {
      continue;
    }
    ReferenceMetadata ref_meta(
        0, 0, std::move(d->meta.GetReference()->GetInformation()));
    Reference ref(0, 0, std::move(*d->meta.GetReference()), version);
    bool found = false;
    for (size_t i = 0; i < references.size(); ++i) {
      if (references[i] == ref) {
        if (reference_meta[i].GetReferenceMetadataValue().empty()) {
          reference_meta[i] = ReferenceMetadata(0, static_cast<uint8_t>(i),
                                                ref_meta.decapsulate());
        } else {
          UTILS_DIE_IF(ref_meta.GetReferenceMetadataValue() !=
                           reference_meta[i].GetReferenceMetadataValue(),
                       "Reference Meta mismatch");
        }

        for (auto& d2 : d->datasets) {
          d2.PatchRefId(ref.GetReferenceId(), static_cast<uint8_t>(i));
        }
        found = true;
        break;
      }
    }
    if (!found) {
      for (auto& d2 : d->datasets) {
        d2.PatchRefId(ref.GetReferenceId(),
                      static_cast<uint8_t>(references.size()));
      }
      ref.PatchRefId(ref.GetReferenceId(),
                     static_cast<uint8_t>(references.size()));
      ref_meta.PatchRefId(ref_meta.GetReferenceId(),
                          static_cast<uint8_t>(references.size()));
      references.emplace_back(std::move(ref));
      reference_meta.emplace_back(std::move(ref_meta));
    }
  }

  for (auto it = reference_meta.begin(); it != reference_meta.end();) {
    if (it->GetReferenceMetadataValue().empty()) {
      it = reference_meta.erase(it);
    } else {
      ++it;
    }
  }

  for (const auto& d : datasets) {
    for (auto& d2 : d->datasets) {
      if (d2.GetHeader().GetDatasetType() !=
          core::parameter::DataUnit::DatasetType::kAligned) {
        continue;
      }

      const auto ref_id = d2.GetHeader().GetReferenceOptions().GetReferenceId();
      for (auto& reference : references) {
        if (ref_id == reference.GetReferenceId()) {
          for (auto& s : reference.GetSequences()) {
            d2.GetHeader().AddRefSequence(ref_id, s.GetId(), 0, 0);
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
void EncapsulatedDatasetGroup::MergeLabels() {
  std::map<std::string, std::vector<LabelDataset>> labels;
  for (const auto& d : datasets) {
    for (auto& l : d->meta.GetLabels()) {
      for (auto& s : d->datasets) {
        labels[l.GetId()].emplace_back(s.GetHeader().GetDatasetId(), l);
      }
    }
  }

  for (auto& [fst, snd] : labels) {
    Label label(fst);
    for (auto& l : snd) {
      label.AddDataset(std::move(l));
    }
    if (label_list == std::nullopt) {
      label_list = LabelList(0);
    }
    label_list->AddLabel(std::move(label));
  }
}

// -----------------------------------------------------------------------------
EncapsulatedDatasetGroup::EncapsulatedDatasetGroup(
    const std::vector<std::string>& input_files,
    core::MpegMinorVersion version) {
  datasets.reserve(input_files.size());
  for (const auto& i : input_files) {
    datasets.emplace_back(std::make_unique<EncapsulatedDataset>(i, version));
  }
  size_t index = 0;
  for (const auto& d : datasets) {
    for (auto& d2 : d->datasets) {
      d2.PatchId(0, static_cast<uint16_t>(index++));
    }
  }

  MergeMetadata(version);

  MergeReferences(version);

  MergeLabels();
}

// -----------------------------------------------------------------------------
DatasetGroup EncapsulatedDatasetGroup::Assemble(
    const core::MpegMinorVersion version) {
  DatasetGroup ret(0, 0, version);
  for (auto& r : references) {
    ret.AddReference(std::move(r));
  }
  for (auto& r : reference_meta) {
    ret.AddReferenceMeta(std::move(r));
  }
  if (label_list != std::nullopt) {
    ret.SetLabels(std::move(*label_list));
  }
  if (group_meta != std::nullopt) {
    ret.SetMetadata(std::move(*group_meta));
  }
  if (group_protection != std::nullopt) {
    ret.SetProtection(std::move(*group_protection));
  }

  for (const auto& d : datasets) {
    for (auto& d2 : d->datasets) {
      ret.AddDataset(std::move(d2));
    }
  }

  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

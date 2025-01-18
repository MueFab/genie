/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_group.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

bool DatasetGroup::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetGroup&>(info);
  return header_ == other.header_ && references_ == other.references_ &&
         reference_metadatas_ == other.reference_metadatas_ &&
         labels_ == other.labels_ && metadata_ == other.metadata_ &&
         protection_ == other.protection_ && dataset_ == other.dataset_;
}

// -----------------------------------------------------------------------------

DatasetGroup::DatasetGroup(util::BitReader& reader,
                           const core::MpegMinorVersion version)
    : version_(version) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  const auto end_pos = start_pos + static_cast<int64_t>(length);
  while (reader.GetStreamPosition() != end_pos) {
    UTILS_DIE_IF(reader.GetStreamPosition() > end_pos,
                 "Read " +
                     std::to_string(reader.GetStreamPosition() - end_pos) +
                     " bytes too far");
    read_box(reader, false);
  }

  UTILS_DIE_IF(header_ == std::nullopt, "Dataset group without header");
}

// -----------------------------------------------------------------------------

DatasetGroup::DatasetGroup(const uint8_t id, const uint8_t version,
                           const core::MpegMinorVersion mpeg_version)
    : header_(DatasetGroupHeader(id, version)), version_(mpeg_version) {}

// -----------------------------------------------------------------------------

void DatasetGroup::AddDataset(Dataset ds) {
  dataset_.emplace_back(std::move(ds));
  header_->AddDatasetId(
      static_cast<uint8_t>(dataset_.back().GetHeader().GetDatasetId()));
}

// -----------------------------------------------------------------------------

const DatasetGroupHeader& DatasetGroup::GetHeader() const { return *header_; }

// -----------------------------------------------------------------------------

const std::vector<Reference>& DatasetGroup::GetReferences() const {
  return references_;
}

// -----------------------------------------------------------------------------

const std::vector<ReferenceMetadata>& DatasetGroup::GetReferenceMetadata()
    const {
  return reference_metadatas_;
}

// -----------------------------------------------------------------------------

bool DatasetGroup::HasLabelList() const { return labels_ != std::nullopt; }

// -----------------------------------------------------------------------------

const LabelList& DatasetGroup::GetLabelList() const { return *labels_; }

// -----------------------------------------------------------------------------

bool DatasetGroup::HasMetadata() const { return metadata_ != std::nullopt; }

// -----------------------------------------------------------------------------

const DatasetGroupMetadata& DatasetGroup::GetMetadata() const {
  return *metadata_;
}

// -----------------------------------------------------------------------------

bool DatasetGroup::HasProtection() const { return protection_ != std::nullopt; }

// -----------------------------------------------------------------------------

const DatasetGroupProtection& DatasetGroup::GetProtection() const {
  return *protection_;
}

// -----------------------------------------------------------------------------

void DatasetGroup::AddMetadata(DatasetGroupMetadata md) {
  metadata_ = std::move(md);
}

// -----------------------------------------------------------------------------

void DatasetGroup::AddProtection(DatasetGroupProtection pr) {
  protection_ = std::move(pr);
}

// -----------------------------------------------------------------------------

const std::vector<Dataset>& DatasetGroup::GetDatasets() const {
  return dataset_;
}

// -----------------------------------------------------------------------------

const std::string& DatasetGroup::GetKey() const {
  static const std::string key = "dgcn";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------

void DatasetGroup::BoxWrite(util::BitWriter& wr) const {
  header_->Write(wr);
  for (const auto& r : references_) {
    r.Write(wr);
  }
  for (const auto& r : reference_metadatas_) {
    r.Write(wr);
  }
  if (labels_ != std::nullopt) {
    labels_->Write(wr);
  }
  if (metadata_ != std::nullopt) {
    metadata_->Write(wr);
  }
  if (protection_ != std::nullopt) {
    protection_->Write(wr);
  }
  for (const auto& d : dataset_) {
    d.Write(wr);
  }
}

// -----------------------------------------------------------------------------

void DatasetGroup::SetMetadata(DatasetGroupMetadata meta) {
  metadata_ = std::move(meta);
}

// -----------------------------------------------------------------------------

void DatasetGroup::SetProtection(DatasetGroupProtection protection) {
  protection_ = std::move(protection);
}

// -----------------------------------------------------------------------------

void DatasetGroup::AddReference(Reference r) {
  references_.emplace_back(std::move(r));
}

// -----------------------------------------------------------------------------

void DatasetGroup::AddReferenceMeta(ReferenceMetadata r) {
  reference_metadatas_.emplace_back(std::move(r));
}

// -----------------------------------------------------------------------------

void DatasetGroup::SetLabels(LabelList l) { labels_ = std::move(l); }

// -----------------------------------------------------------------------------

void DatasetGroup::PatchId(const uint8_t group_id) {
  if (header_ != std::nullopt) {
    header_->PatchId(group_id);
  }
  for (auto& r : references_) {
    r.PatchId(group_id);
  }
  for (auto& r : reference_metadatas_) {
    r.PatchId(group_id);
  }
  if (labels_ != std::nullopt) {
    labels_->PatchId(group_id);
  }
  if (metadata_ != std::nullopt) {
    metadata_->PatchId(group_id);
  }
  if (protection_ != std::nullopt) {
    protection_->PatchId(group_id);
  }
  for (auto& d : dataset_) {
    d.PatchId(group_id, d.GetHeader().GetDatasetId());
  }
}

// -----------------------------------------------------------------------------

void DatasetGroup::PatchRefId(const uint8_t old_id,  // NOLINT
                              const uint8_t new_id) {
  for (auto& r : references_) {
    r.PatchRefId(old_id, new_id);
  }
  for (auto& r : reference_metadatas_) {
    r.PatchRefId(old_id, new_id);
  }
  for (auto& d : dataset_) {
    d.PatchRefId(old_id, new_id);
  }
}

// -----------------------------------------------------------------------------

std::vector<Reference>& DatasetGroup::GetReferences() { return references_; }

// -----------------------------------------------------------------------------

std::vector<ReferenceMetadata>& DatasetGroup::GetReferenceMetadata() {
  return reference_metadatas_;
}

// -----------------------------------------------------------------------------

LabelList& DatasetGroup::GetLabelList() { return *labels_; }

// -----------------------------------------------------------------------------

DatasetGroupMetadata& DatasetGroup::GetMetadata() { return *metadata_; }

// -----------------------------------------------------------------------------

DatasetGroupProtection& DatasetGroup::GetProtection() { return *protection_; }

// -----------------------------------------------------------------------------

std::vector<Dataset>& DatasetGroup::GetDatasets() { return dataset_; }

// -----------------------------------------------------------------------------

void DatasetGroup::PrintDebug(std::ostream& output, const uint8_t depth,
                               const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Dataset Group");
  header_->PrintDebug(output, depth + 1, max_depth);
  for (const auto& r : references_) {
    r.PrintDebug(output, depth + 1, max_depth);
  }
  for (const auto& r : reference_metadatas_) {
    r.PrintDebug(output, depth + 1, max_depth);
  }
  if (labels_) {
    labels_->PrintDebug(output, depth + 1, max_depth);
  }
  if (metadata_) {
    metadata_->PrintDebug(output, depth + 1, max_depth);
  }
  if (protection_) {
    metadata_->PrintDebug(output, depth + 1, max_depth);
  }
  for (const auto& r : dataset_) {
    r.PrintDebug(output, depth + 1, max_depth);
  }
}

// -----------------------------------------------------------------------------

void DatasetGroup::read_box(util::BitReader& reader,  // NOLINT
                            const bool in_offset) {
  std::string tmp_str(4, '\0');
  reader.ReadAlignedBytes(tmp_str.data(), tmp_str.length());
  if (tmp_str == "dghd") {  // NOLINT
    UTILS_DIE_IF(header_ != std::nullopt, "More than one header");
    UTILS_DIE_IF(!reference_metadatas_.empty(),
                 "Header must be before ref metadata");
    UTILS_DIE_IF(labels_ != std::nullopt, "Header must be before labels");
    UTILS_DIE_IF(metadata_ != std::nullopt, "Header must be before metadata");
    UTILS_DIE_IF(protection_ != std::nullopt,
                 "Header must be before protection");
    UTILS_DIE_IF(!dataset_.empty(), "Header must be before dataset");
    header_ = DatasetGroupHeader(reader);
  } else if (tmp_str == "rfgn") {  // NOLINT
    UTILS_DIE_IF(!reference_metadatas_.empty(),
                 "Reference must be before ref metadata");
    UTILS_DIE_IF(labels_ != std::nullopt, "Reference must be before labels");
    UTILS_DIE_IF(metadata_ != std::nullopt,
                 "Reference must be before metadata");
    UTILS_DIE_IF(protection_ != std::nullopt,
                 "Reference must be before protection");
    UTILS_DIE_IF(!dataset_.empty(), "Reference must be before dataset");
    references_.emplace_back(reader, version_);
  } else if (tmp_str == "rfmd") {  // NOLINT
    UTILS_DIE_IF(labels_ != std::nullopt, "Ref metadata must be before labels");
    UTILS_DIE_IF(metadata_ != std::nullopt,
                 "Ref metadata must be before metadata");
    UTILS_DIE_IF(protection_ != std::nullopt,
                 "Ref metadata must be before protection");
    UTILS_DIE_IF(!dataset_.empty(), "Ref metadata must be before dataset");
    reference_metadatas_.emplace_back(reader);
  } else if (tmp_str == "labl") {  // NOLINT
    UTILS_DIE_IF(labels_ != std::nullopt, "Labels already present");
    UTILS_DIE_IF(metadata_ != std::nullopt, "Labels must be before metadata");
    UTILS_DIE_IF(protection_ != std::nullopt,
                 "Labels must be before protection");
    UTILS_DIE_IF(!dataset_.empty(), "Labels must be before dataset");
    labels_.emplace(reader);
  } else if (tmp_str == "dgmd") {  // NOLINT
    UTILS_DIE_IF(metadata_ != std::nullopt, "Metadata already present");
    UTILS_DIE_IF(protection_ != std::nullopt,
                 "Metadata must be before protection");
    UTILS_DIE_IF(!dataset_.empty(), "Metadata must be before dataset");
    metadata_ = DatasetGroupMetadata(reader, version_);
  } else if (tmp_str == "dgpr") {  // NOLINT
    UTILS_DIE_IF(protection_ != std::nullopt, "Protection already present");
    UTILS_DIE_IF(!dataset_.empty(), "Protection must be before dataset");
    protection_ = DatasetGroupProtection(reader, version_);
  } else if (tmp_str == "dtcn") {  // NOLINT
    dataset_.emplace_back(reader, version_);
  } else if (tmp_str == "offs") {
    UTILS_DIE_IF(in_offset, "Recursive offset not permitted");
    reader.ReadAlignedBytes(tmp_str.data(), tmp_str.length());
    const auto offset = reader.ReadAlignedInt<uint64_t>();
    if (offset == ~static_cast<uint64_t>(0)) {
      return;
    }
    const auto pos_save = reader.GetStreamPosition();
    reader.SetStreamPosition(static_cast<int64_t>(offset));
    read_box(reader, true);
    reader.SetStreamPosition(pos_save);
  } else {
    UTILS_DIE("Unknown box");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

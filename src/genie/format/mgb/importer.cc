/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/importer.h"

#include <string>
#include <utility>

#include "genie/format/mgb/access_unit.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Mgb";

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
Importer::Importer(std::istream& file, core::ReferenceManager* manager,
                   core::RefDecoder* ref_decoder, const bool ref_only)
    : ReferenceSource(manager),
      reader_(file),
      factory_(manager, this, ref_only),
      ref_manager_(manager),
      decoder_(ref_decoder),
      file_size_(0),
      last_progress_(0) {
  const auto pos = file.tellg();
  file.seekg(0, std::ios::end);
  file_size_ = file.tellg();
  file.seekg(pos);
}

// -----------------------------------------------------------------------------
bool Importer::Pump(uint64_t& id, std::mutex&) {
  // util::Watch watch; TODO(fabian): Statistics
  std::optional<AccessUnit> unit;
  util::Section sec{};
  {
    std::unique_lock lock_guard(lock_);
    unit = factory_.read(reader_);
    if (!unit) {
      return false;
    }
    sec.start = id;
    sec.length = unit->GetHeader().GetReadCount();
    id += unit->GetHeader().GetReadCount();

    const float progress = static_cast<float>(reader_.GetStreamPosition()) /
                           static_cast<float>(file_size_);
    while (progress - last_progress_ > 0.05) {  // NOLINT
      last_progress_ += 0.05;
      UTILS_LOG(util::Logger::Severity::INFO,
                "Progress: " +
                    std::to_string(
                        static_cast<int>(std::round(last_progress_ * 100))) +
                    "% of file read");
    }
  }
  FlowOut(ConvertAu(std::move(unit.value())), sec);
  return true;
}

// -----------------------------------------------------------------------------
std::string Importer::GetRef(const bool raw, const size_t f_pos,
                             const size_t start, const size_t end) {
  AccessUnit au(0, 0, core::record::ClassType::kNone, 0,
                core::parameter::DataUnit::DatasetType::kReference, 0, false,
                core::AlphabetId::kAcgtn);
  std::string ret;
  {
    std::lock_guard f_lock(this->lock_);
    const size_t old_pos = reader_.GetStreamPosition();

    if (raw) {
      reader_.SetStreamPosition(f_pos + start);
      ret.resize(end - start);
      reader_.ReadAlignedBytes(&ret[0], ret.length());
    } else {
      reader_.SetStreamPosition(f_pos);
      au = AccessUnit(factory_.GetParams(), reader_, false);
    }

    reader_.SetStreamPosition(old_pos);
  }

  if (!raw) {
    ret = decoder_->Decode(ConvertAu(std::move(au)));
  }

  return ret;
}

// -----------------------------------------------------------------------------
core::AccessUnit Importer::ConvertAu(AccessUnit&& au) const {
  auto unit = std::move(au);
  auto paramset = factory_.GetParams(unit.GetHeader().GetParameterId());
  core::AccessUnit set(std::move(paramset), unit.GetHeader().GetReadCount());

  for (auto& b : unit.GetBlocks()) {
    set.Set(static_cast<core::GenDesc>(b.GetDescriptorId()), b.MovePayload());
  }
  if (unit.GetHeader().GetClass() != core::record::ClassType::kClassU) {
    set.SetReference(unit.GetHeader().GetAlignmentInfo().GetRefId());
    set.SetMinPos(unit.GetHeader().GetAlignmentInfo().GetStartPos());
    set.SetMaxPos(unit.GetHeader().GetAlignmentInfo().GetEndPos());
    ref_manager_->ValidateRefId(set.GetReference());
  } else {
    set.SetReference(0);
    set.SetMinPos(0);
    set.SetMaxPos(0);
  }
  set.SetNumReads(unit.GetHeader().GetReadCount());
  set.SetClassType(unit.GetHeader().GetClass());
  if (unit.GetHeader().GetClass() != core::record::ClassType::kClassU &&
      !set.GetParameters().IsComputedReference()) {
    auto seqs = ref_manager_->GetSequences();
    if (const auto cur_seq = ref_manager_->Id2Ref(
            unit.GetHeader().GetAlignmentInfo().GetRefId());
        std::find(seqs.begin(), seqs.end(), cur_seq) != seqs.end()) {
      set.SetReference(
          ref_manager_->Load(
              cur_seq, unit.GetHeader().GetAlignmentInfo().GetStartPos(),
              unit.GetHeader().GetAlignmentInfo().GetEndPos() + 1),
          {});
    }
  }
  return set;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

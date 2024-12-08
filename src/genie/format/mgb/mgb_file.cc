/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/mgb_file.h"

#include <algorithm>
#include <memory>
#include <set>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
uint8_t MgbFile::data_unit_order(const core::parameter::DataUnit& u) {
  switch (u.GetDataUnitType()) {
    case core::parameter::DataUnit::DataUnitType::kParameterSet:
      return 0;
    case core::parameter::DataUnit::DataUnitType::kRawReference:
      return 1;
    case core::parameter::DataUnit::DataUnitType::kAccessUnit:
      return 2;
  }
  return 255;
}

// -----------------------------------------------------------------------------
void MgbFile::Write(util::BitWriter& writer) const {
  for (const auto& [fst, snd] : units_) {
    snd->Write(writer);
  }
}

// -----------------------------------------------------------------------------
void MgbFile::AddUnit(std::unique_ptr<core::parameter::DataUnit> unit) {
  units_.emplace_back(0, std::move(unit));
}

// -----------------------------------------------------------------------------
MgbFile::MgbFile() : file_(nullptr) {}

// -----------------------------------------------------------------------------
MgbFile::MgbFile(std::istream* file)
    : file_(file), reader_(std::make_unique<util::BitReader>(*file_)) {
  while (true) {
    uint64_t pos = reader_->GetStreamPosition();
    const auto unit_type =
        reader_->ReadAlignedInt<core::parameter::DataUnit::DataUnitType>();
    if (!reader_->IsStreamGood()) {
      break;
    }

    switch (unit_type) {
      case core::parameter::DataUnit::DataUnitType::kParameterSet: {
        auto set = std::make_unique<core::parameter::ParameterSet>(*reader_);
        parameter_sets_.emplace(set->GetId(), set->GetEncodingSet());
        units_.emplace_back(pos, std::move(set));
      } break;
      case core::parameter::DataUnit::DataUnitType::kAccessUnit:
        units_.emplace_back(
            pos, std::make_unique<AccessUnit>(parameter_sets_, *reader_));
        break;
      case core::parameter::DataUnit::DataUnitType::kRawReference:
        units_.emplace_back(pos, std::make_unique<RawReference>(*reader_));
        break;
      default:
        UTILS_DIE("Unknown data unit");
    }
  }
  file_->clear();
  file_->seekg(0, std::ios::beg);
}

// -----------------------------------------------------------------------------
void MgbFile::print_debug(std::ostream& output, const uint8_t max_depth) const {
  for (const auto& [fst, snd] : units_) {
    snd->PrintDebug(output, 0, max_depth);
  }
}

// -----------------------------------------------------------------------------
void MgbFile::sort_by_class() {
  auto sorter =
      [](const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>&
             u1,
         const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>&
             u2) -> bool {
    return base_sorter(
        u1, u2, [](const AccessUnit& a1, const AccessUnit& a2) -> bool {
          if (a1.GetHeader().GetClass() != a2.GetHeader().GetClass()) {
            return static_cast<uint8_t>(a1.GetHeader().GetClass()) <
                   static_cast<uint8_t>(a2.GetHeader().GetClass());
          }
          if (a1.GetHeader().GetClass() == core::record::ClassType::kClassU &&
              a2.GetHeader().GetClass() == core::record::ClassType::kClassU) {
            return a1.GetHeader().GetId() < a2.GetHeader().GetId();
          }
          if (a1.GetHeader().GetClass() == core::record::ClassType::kClassU) {
            return false;
          }
          if (a1.GetHeader().GetAlignmentInfo().GetRefId() !=
              a2.GetHeader().GetAlignmentInfo().GetRefId()) {
            return a1.GetHeader().GetAlignmentInfo().GetRefId() <
                   a2.GetHeader().GetAlignmentInfo().GetRefId();
          }
          if (a1.GetHeader().GetAlignmentInfo().GetStartPos() !=
              a2.GetHeader().GetAlignmentInfo().GetStartPos()) {
            return a1.GetHeader().GetAlignmentInfo().GetStartPos() <
                   a2.GetHeader().GetAlignmentInfo().GetStartPos();
          }
          return a1.GetHeader().GetAlignmentInfo().GetEndPos() <
                 a2.GetHeader().GetAlignmentInfo().GetEndPos();
        });
  };
  std::sort(units_.begin(), units_.end(), sorter);
}

// -----------------------------------------------------------------------------
void MgbFile::sort_by_position() {
  auto sorter =
      [](const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>&
             u1,
         const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>&
             u2) -> bool {
    return base_sorter(
        u1, u2, [](const AccessUnit& a1, const AccessUnit& a2) -> bool {
          if (a1.GetHeader().GetClass() == core::record::ClassType::kClassU &&
              a2.GetHeader().GetClass() == core::record::ClassType::kClassU) {
            return a1.GetHeader().GetId() < a2.GetHeader().GetId();
          }
          if (a1.GetHeader().GetClass() == core::record::ClassType::kClassU) {
            return false;
          }
          if (a2.GetHeader().GetClass() == core::record::ClassType::kClassU) {
            return true;
          }
          if (a1.GetHeader().GetAlignmentInfo().GetRefId() !=
              a2.GetHeader().GetAlignmentInfo().GetRefId()) {
            return a1.GetHeader().GetAlignmentInfo().GetRefId() <
                   a2.GetHeader().GetAlignmentInfo().GetRefId();
          }
          if (a1.GetHeader().GetAlignmentInfo().GetStartPos() !=
              a2.GetHeader().GetAlignmentInfo().GetStartPos()) {
            return a1.GetHeader().GetAlignmentInfo().GetStartPos() <
                   a2.GetHeader().GetAlignmentInfo().GetStartPos();
          }
          if (a1.GetHeader().GetAlignmentInfo().GetEndPos() !=
              a2.GetHeader().GetAlignmentInfo().GetEndPos()) {
            return a1.GetHeader().GetAlignmentInfo().GetEndPos() <
                   a2.GetHeader().GetAlignmentInfo().GetEndPos();
          }
          return static_cast<uint8_t>(a1.GetHeader().GetClass()) <
                 static_cast<uint8_t>(a2.GetHeader().GetClass());
        });
  };
  std::sort(units_.begin(), units_.end(), sorter);
}

// -----------------------------------------------------------------------------
void MgbFile::remove_class(core::record::ClassType type) {
  auto remover =
      [type](
          const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>&
              u1) -> bool {
    if (u1.second->GetDataUnitType() !=
        core::parameter::DataUnit::DataUnitType::kAccessUnit) {
      return false;
    }
    return dynamic_cast<const AccessUnit&>(*u1.second).GetHeader().GetClass() ==
           type;
  };
  units_.erase(std::remove_if(units_.begin(), units_.end(), remover),
               units_.end());
}

// -----------------------------------------------------------------------------
void MgbFile::select_mapping_range(uint16_t ref_id, uint64_t start_pos,
                                   uint64_t end_pos) {
  auto remover =
      [ref_id, start_pos, end_pos](
          const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>&
              u1) -> bool {
    if (u1.second->GetDataUnitType() !=
        core::parameter::DataUnit::DataUnitType::kAccessUnit) {
      return false;
    }

    if (dynamic_cast<const AccessUnit&>(*u1.second).GetHeader().GetClass() ==
        core::record::ClassType::kClassU) {
      return false;
    }

    return dynamic_cast<const AccessUnit&>(*u1.second)
                   .GetHeader()
                   .GetAlignmentInfo()
                   .GetRefId() != ref_id ||
           dynamic_cast<const AccessUnit&>(*u1.second)
                   .GetHeader()
                   .GetAlignmentInfo()
                   .GetStartPos() < start_pos ||
           dynamic_cast<const AccessUnit&>(*u1.second)
                   .GetHeader()
                   .GetAlignmentInfo()
                   .GetEndPos() >= end_pos;
  };
  units_.erase(std::remove_if(units_.begin(), units_.end(), remover),
               units_.end());
}

// -----------------------------------------------------------------------------
void MgbFile::remove_unused_parametersets() {
  std::set<uint8_t> ids;
  for (const auto& [fst, snd] : units_) {
    if (snd->GetDataUnitType() ==
        core::parameter::DataUnit::DataUnitType::kAccessUnit) {
      ids.emplace(
          dynamic_cast<const AccessUnit&>(*snd).GetHeader().GetParameterId());
    }
  }

  units_.erase(
      std::remove_if(
          units_.begin(), units_.end(),
          [&ids](const std::pair<
                 uint64_t, std::unique_ptr<core::parameter::DataUnit>>& u1)
              -> bool {
            if (u1.second->GetDataUnitType() !=
                core::parameter::DataUnit::DataUnitType::kParameterSet) {
              return false;
            }
            if (ids.find(dynamic_cast<const core::parameter::ParameterSet&>(
                             *u1.second)
                             .GetId()) == ids.end()) {
              return true;
            }
            return false;
          }),
      units_.end());
}

// -----------------------------------------------------------------------------
std::vector<Block> MgbFile::ExtractDescriptor(
    const core::record::ClassType type, core::GenDesc descriptor,
    const std::vector<uint8_t>& param_sets) {
  std::vector<Block> ret;
  for (auto& [fst, snd] : units_) {
    if (snd->GetDataUnitType() ==
        core::parameter::DataUnit::DataUnitType::kAccessUnit) {
      auto& au = dynamic_cast<AccessUnit&>(*snd);
      if (std::find(param_sets.begin(), param_sets.end(),
                    au.GetHeader().GetParameterId()) == param_sets.end()) {
        continue;
      }
      if (au.GetHeader().GetClass() != type) {
        continue;
      }
      bool found = false;
      for (auto& a : au.GetBlocks()) {
        if (static_cast<core::GenDesc>(a.GetDescriptorId()) == descriptor) {
          found = true;
          ret.emplace_back(std::move(a));
          break;
        }
      }
      if (!found) {
        ret.emplace_back(static_cast<uint8_t>(descriptor),
                         core::AccessUnit::Descriptor(descriptor));
      }
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
void MgbFile::ClearAuBlocks(const std::vector<uint8_t>& param_sets) {
  for (auto& [fst, snd] : units_) {
    if (snd->GetDataUnitType() ==
        core::parameter::DataUnit::DataUnitType::kAccessUnit) {
      if (auto& au = dynamic_cast<AccessUnit&>(*snd);
          std::find(param_sets.begin(), param_sets.end(),
                    au.GetHeader().GetParameterId()) != param_sets.end()) {
        au.GetBlocks().clear();
      }
    }
  }
}

// -----------------------------------------------------------------------------
std::vector<std::unique_ptr<core::parameter::ParameterSet>>
MgbFile::ExtractParameters(const std::vector<uint8_t>& param_sets) {
  std::vector<std::unique_ptr<core::parameter::ParameterSet>> ret;
  for (auto it = units_.begin(); it != units_.end();) {
    if (it->second->GetDataUnitType() ==
        core::parameter::DataUnit::DataUnitType::kParameterSet) {
      if (std::find(
              param_sets.begin(), param_sets.end(),
              dynamic_cast<core::parameter::ParameterSet*>(it->second.get())
                  ->GetId()) != param_sets.end()) {
        ret.emplace_back(
            dynamic_cast<core::parameter::ParameterSet*>(it->second.release()));
        it = units_.erase(it);
      } else {
        ++it;
      }
    } else {
      ++it;
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
std::vector<std::unique_ptr<AccessUnit>> MgbFile::ExtractAUs(
    const std::vector<uint8_t>& param_sets) {
  std::vector<std::unique_ptr<AccessUnit>> ret;
  for (auto it = units_.begin(); it != units_.end();) {
    if (it->second->GetDataUnitType() ==
        core::parameter::DataUnit::DataUnitType::kAccessUnit) {
      if (std::find(param_sets.begin(), param_sets.end(),
                    dynamic_cast<AccessUnit*>(it->second.get())
                        ->GetHeader()
                        .GetParameterId()) != param_sets.end()) {
        ret.emplace_back(dynamic_cast<AccessUnit*>(it->second.release()));
        it = units_.erase(it);
      } else {
        ++it;
      }
    } else {
      ++it;
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
std::vector<uint8_t> MgbFile::collect_param_ids(
    const bool multiple_alignments, const bool pos40,
    const core::parameter::DataUnit::DatasetType dataset_type,
    const core::AlphabetId alphabet) const {
  std::vector<uint8_t> ret;
  for (const auto& [fst, snd] : parameter_sets_) {
    if (snd.HasMultipleAlignments() == multiple_alignments &&
        (snd.GetPosSize() == 40) == pos40 &&  // NOLINT
        snd.GetDatasetType() == dataset_type &&
        snd.GetAlphabetId() == alphabet) {
      ret.emplace_back(static_cast<uint8_t>(fst));
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/sam/sam_to_mgrec/sam_group.h"

// -----------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/record/alignment_split/unpaired.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------
std::tuple<bool, uint8_t> SamRecordGroup::ConvertFlags2Mpeg(
    const uint16_t flags) {
  uint8_t flags_mpeg = 0;

  flags_mpeg |= (flags & BAM_FDUP) >> 10u;    // PCR / duplicate
  flags_mpeg |= (flags & BAM_FQCFAIL) >> 8u;  // vendor quality check failed
  flags_mpeg |= (flags & BAM_FPROPER_PAIR) << 1u;  // proper pair

  bool rcomp = flags & BAM_FREVERSE;  // rcomp

  return std::make_tuple(rcomp, flags_mpeg);
}

// -----------------------------------------------------------------------------
void SamRecordGroup::AddAlignment(core::record::Record& rec, SamRecord* r1,
                                  SamRecord* r2, const bool paired_end,
                                  const bool force_split) {  // NOLINT
  UTILS_DIE_IF(r1 == nullptr && r2 == nullptr, "Both SAM records are nullptr");
  const SamRecord* base_rec =
      r1 ? r1 : r2;  // Record to derive name and flags from
  if (r1 && r2 && r1->IsUnmapped()) {
    base_rec = r2;
  }
  core::record::Alignment alignment(base_rec->GetECigar(),
                                    base_rec->IsReverse());
  alignment.AddMappingScore(base_rec->GetMapq());

  core::record::AlignmentBox alignment_container(base_rec->GetPos(),
                                                 std::move(alignment));

  // Single end or class HM (only possible if both SAM records available ) ->
  // No split alignment
  if (!paired_end || (r1 && r2 && (r1->IsUnmapped() || r2->IsUnmapped()))) {
    rec.AddAlignment(base_rec->GetRid(), std::move(alignment_container));
    return;
  }

  if (r1 == nullptr || r2 == nullptr) {
    // Only one SAM record is unavailable
    if (const auto r_avail = r1 ? r1 : r2;
        (r_avail->mate_rid_ == r_avail->GetRid() &&
         r_avail->mate_pos_ == r_avail->GetPos()) ||
        r_avail->mate_rid_ == -1) {
      // Case 1: Paired SAM record is missing, switch to unpaired.
      // TODO(fabian): Maybe still preserve pairing information?
      auto split_align =
          std::make_unique<core::record::alignment_split::Unpaired>();
      alignment_container.AddAlignmentSplit(std::move(split_align));
    } else {
      // Case 2: Paired SAM record is mapped to other reference sequence,
      // no way to check if data is actually there
      auto split_align =
          std::make_unique<core::record::alignment_split::OtherRec>(
              r_avail->mate_pos_, r_avail->mate_rid_);
      alignment_container.AddAlignmentSplit(std::move(split_align));
    }

  } else {
    // Both SAM records are available
    if (force_split) {  // NOLINT  // TODO(fabian): Split automatically if
                        // delta > 32767
                        // Case 1: split into two MPEG records
                        // auto splitAlign =  // NOLINT
      // std::make_unique<core::record::alignment_split::OtherRec>(r2->getPos()
      // - 1, r2->getRID());  // NOLINT
      // alignmentContainer.addAlignmentSplit(std::move(splitAlign));  //
      // NOLINT
    } else {
      // Case 2: Merge into one MPEG record
      core::record::Alignment alignment2(r2->GetECigar(), r2->IsReverse());
      alignment2.AddMappingScore(r2->GetMapq());

      auto delta = r2->GetPos() - r1->GetPos();
      auto split_align =
          std::make_unique<core::record::alignment_split::SameRec>(
              delta, std::move(alignment2));

      alignment_container.AddAlignmentSplit(std::move(split_align));
    }
  }

  // Append alignment to record
  rec.AddAlignment(base_rec->GetRid(), std::move(alignment_container));
}

// -----------------------------------------------------------------------------
SamRecordGroup::SamRecordGroup()
    : data_(static_cast<size_t>(TemplateType::TOTAL_INDICES),
            std::vector<std::list<SamRecord>>(
                static_cast<size_t>(MappingType::TOTAL_INDICES))) {}

// -----------------------------------------------------------------------------
void SamRecordGroup::AddRecord(SamRecord&& rec) {
  auto template_type = [&rec] {
    if (!rec.IsPaired()) {
      return TemplateType::SINGLE;
    }
    if (rec.IsRead1()) {
      return TemplateType::PAIRED_1;
    }
    if (rec.IsRead2()) {
      return TemplateType::PAIRED_2;
    }
    return TemplateType::PAIRED_UNKNOWN;
  }();

  auto mapping_type = [&rec] {
    if (rec.IsUnmapped()) {
      return MappingType::UNMAPPED;
    }
    if (rec.IsPrimary()) {
      return MappingType::PRIMARY;
    }
    return MappingType::NONPRIMARY;
  }();

  // Put SAM record into the bucket for its mapping case
  data_[static_cast<uint8_t>(template_type)][static_cast<uint8_t>(mapping_type)]
      .push_back(std::move(rec));
}

// -----------------------------------------------------------------------------
bool SamRecordGroup::CheckIfPaired() const {
  size_t count_single_end = 0;
  size_t count_paired_end = 0;

  for (auto& a : data_[static_cast<uint8_t>(TemplateType::SINGLE)]) {
    count_single_end += a.size();
  }
  for (auto& a : data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]) {
    count_paired_end += a.size();
  }
  for (auto& a : data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]) {
    count_paired_end += a.size();
  }
  for (auto& a : data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]) {
    count_paired_end += a.size();
  }

  UTILS_DIE_IF(count_single_end > 0 && count_paired_end > 0,
               "Paired and unpaired records in same SAM template");

  return count_paired_end > 0;
}

// -----------------------------------------------------------------------------
core::record::ClassType SamRecordGroup::CheckClassTypeSingle() {
  const size_t primary_count =
      data_[static_cast<uint8_t>(TemplateType::SINGLE)]
           [static_cast<uint8_t>(MappingType::PRIMARY)]
               .size() +
      data_[static_cast<uint8_t>(TemplateType::SINGLE)]
           [static_cast<uint8_t>(MappingType::UNMAPPED)]
               .size();
  UTILS_DIE_IF(primary_count > 1,
               "Multiple primary alignments in single ended SAM record");
  if (primary_count == 0) {
    // std::cerr << "Warning: Single ended SAM record without primary reads,
    // discarding whole record" << std::endl;
    data_.clear();
    return core::record::ClassType::kNone;
  }
  if (data_[static_cast<uint8_t>(TemplateType::SINGLE)]
           [static_cast<uint8_t>(MappingType::PRIMARY)]
               .empty() &&
      !data_[static_cast<uint8_t>(TemplateType::SINGLE)]
            [static_cast<uint8_t>(MappingType::NONPRIMARY)]
                .empty()) {
    // std::cerr << "Warning: Single ended SAM record without primary
    // alignment, but with secondary alignments, "
    //              "discarding "
    //              "alignments"
    //           << std::endl;
    data_[static_cast<uint8_t>(TemplateType::SINGLE)]
         [static_cast<uint8_t>(MappingType::NONPRIMARY)]
             .clear();
  }
  return data_[static_cast<uint8_t>(TemplateType::SINGLE)]
              [static_cast<uint8_t>(MappingType::PRIMARY)]
                  .empty()
             ? core::record::ClassType::kClassU
             : core::record::ClassType::kClassI;
}

// -----------------------------------------------------------------------------
void SamRecordGroup::RemoveDuplicatesPaired(TemplateType temp_type,
                                            MappingType map_type) {
  while (data_[static_cast<uint8_t>(temp_type)][static_cast<uint8_t>(map_type)]
             .size() > 1) {
    if (data_[static_cast<uint8_t>(temp_type)][static_cast<uint8_t>(map_type)]
            .back() ==
        data_[static_cast<uint8_t>(temp_type)][static_cast<uint8_t>(map_type)]
            .front()) {
      data_[static_cast<uint8_t>(temp_type)][static_cast<uint8_t>(map_type)]
          .pop_back();
    } else {
      UTILS_DIE("Too many Pair_1 reads");
    }
  }
}

// -----------------------------------------------------------------------------
size_t SamRecordGroup::PrimaryTemplateCount(TemplateType temp_type) const {
  return data_[static_cast<uint8_t>(temp_type)]
              [static_cast<uint8_t>(MappingType::PRIMARY)]
                  .size() +
         data_[static_cast<uint8_t>(temp_type)]
              [static_cast<uint8_t>(MappingType::UNMAPPED)]
                  .size();
}

// -----------------------------------------------------------------------------
size_t SamRecordGroup::MappingCount(MappingType map_type) const {  // NOLINT
  return data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
              [static_cast<uint8_t>(map_type)]
                  .size() +
         data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
              [static_cast<uint8_t>(map_type)]
                  .size() +
         data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]
              [static_cast<uint8_t>(map_type)]
                  .size();
}

// -----------------------------------------------------------------------------
void SamRecordGroup::RemoveAmbiguousSecondaryAlignments() {
  // Get rid of unknown pair index
  if (data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]
           [static_cast<uint8_t>(MappingType::PRIMARY)]
               .size() > 1 &&
      !data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]
            [static_cast<uint8_t>(MappingType::NONPRIMARY)]
                .empty()) {
    std::cerr << "Warning: Secondary alignments without read ordering are "
                 "discarded"
              << std::endl;
    data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]
         [static_cast<uint8_t>(MappingType::NONPRIMARY)]
             .clear();
  }
}

// -----------------------------------------------------------------------------
void SamRecordGroup::MoveSecondaryAlignments() {
  // Move secondary alignments which can be associated with a primary read
  auto s1 = std::move(data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
                           [static_cast<uint8_t>(MappingType::NONPRIMARY)]);
  auto s2 = std::move(data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
                           [static_cast<uint8_t>(MappingType::NONPRIMARY)]);
  for (auto it1 = s1.begin(); it1 != s1.end();) {
    bool found = false;
    for (auto it2 = s2.begin(); it2 != s2.end(); ++it2) {
      if (it1->IsPairOf(*it2)) {
        found = true;
        data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
             [static_cast<uint8_t>(MappingType::NONPRIMARY)]
                 .push_back(*it1);
        data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
             [static_cast<uint8_t>(MappingType::NONPRIMARY)]
                 .push_back(*it2);
        it1 = s1.erase(it1);
        s2.erase(it2);
        break;
      }
    }
    if (!found) {
      ++it1;
    }
  }

  // Move secondary alignments which can not be associated
  for (auto& it1 : s1) {
    data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
         [static_cast<uint8_t>(MappingType::NONPRIMARY)]
             .push_back(it1);
  }
  for (auto& it2 : s2) {
    data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
         [static_cast<uint8_t>(MappingType::NONPRIMARY)]
             .push_back(it2);
  }
}

// -----------------------------------------------------------------------------
void SamRecordGroup::GuessUnknownReadOrder(size_t& unknown_count,
                                           size_t& read_1_count,
                                           size_t& read_2_count) {
  // Get rid of unknown pair indices
  while (unknown_count > 0) {
    const auto index = data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]
                            [static_cast<uint8_t>(MappingType::PRIMARY)]
                                .empty()
                           ? static_cast<uint8_t>(MappingType::UNMAPPED)
                           : static_cast<uint8_t>(MappingType::PRIMARY);
    if (read_1_count == 0) {
      data_[static_cast<uint8_t>(TemplateType::PAIRED_1)][index].push_back(
          data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)][index]
              .back());
      data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)][index]
          .pop_back();
      if (index == static_cast<uint8_t>(MappingType::PRIMARY)) {
        auto& vec = data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
                         [static_cast<uint8_t>(MappingType::NONPRIMARY)];
        auto& vec2 = data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]
                          [static_cast<uint8_t>(MappingType::NONPRIMARY)];
        vec.insert(vec.end(), vec2.begin(), vec2.end());
        vec2.clear();
      }
      read_1_count++;
      unknown_count--;
    } else if (read_2_count == 0) {
      data_[static_cast<uint8_t>(TemplateType::PAIRED_2)][index].push_back(
          data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)][index]
              .back());
      data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)][index]
          .pop_back();
      if (index == static_cast<uint8_t>(MappingType::PRIMARY)) {
        auto& vec = data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
                         [static_cast<uint8_t>(MappingType::NONPRIMARY)];
        auto& vec2 = data_[static_cast<uint8_t>(TemplateType::PAIRED_UNKNOWN)]
                          [static_cast<uint8_t>(MappingType::NONPRIMARY)];
        vec.insert(vec.end(), vec2.begin(), vec2.end());
        vec2.clear();
      }
      read_2_count++;
      unknown_count--;
    }
  }

  MoveSecondaryAlignments();
}

// -----------------------------------------------------------------------------
core::record::ClassType SamRecordGroup::CheckClassTypePaired() {
  RemoveDuplicatesPaired(TemplateType::PAIRED_1, MappingType::PRIMARY);
  RemoveDuplicatesPaired(TemplateType::PAIRED_1, MappingType::UNMAPPED);
  RemoveDuplicatesPaired(TemplateType::PAIRED_2, MappingType::PRIMARY);
  RemoveDuplicatesPaired(TemplateType::PAIRED_2, MappingType::UNMAPPED);

  size_t read_1_count = PrimaryTemplateCount(TemplateType::PAIRED_1);
  size_t read_2_count = PrimaryTemplateCount(TemplateType::PAIRED_2);
  size_t unknown_count = PrimaryTemplateCount(TemplateType::PAIRED_UNKNOWN);
  const size_t primary_count = read_1_count + read_2_count + unknown_count;
  UTILS_DIE_IF(primary_count > 2,
               "More than 2 primary reads in paired sam record");

  if (primary_count == 0) {
    // std::cerr << "Warning: Paired-end SAM record without primary reads,
    // discarding whole record" << std::endl;
    data_.clear();
    return core::record::ClassType::kNone;
  }

  RemoveAmbiguousSecondaryAlignments();
  GuessUnknownReadOrder(unknown_count, read_1_count, read_2_count);

  const size_t unmapped_count = MappingCount(MappingType::UNMAPPED);

  if (unmapped_count == 0) {
    return core::record::ClassType::kClassI;
  }
  if (unmapped_count == read_1_count + read_2_count) {
    return core::record::ClassType::kClassU;
  }
  return core::record::ClassType::kClassHm;
}

// -----------------------------------------------------------------------------
std::pair<bool, core::record::ClassType> SamRecordGroup::validate() {
  if (const bool paired = CheckIfPaired(); !paired) {
    return std::make_pair(false, CheckClassTypeSingle());
  }
  return std::make_pair(true, CheckClassTypePaired());
}

// -----------------------------------------------------------------------------
std::pair<SamRecord*, SamRecord*> SamRecordGroup::GetReadTuple() {
  SamRecord* r1 = nullptr;
  SamRecord* r2 = nullptr;

  if (!data_[static_cast<uint8_t>(TemplateType::SINGLE)]
            [static_cast<uint8_t>(MappingType::PRIMARY)]
                .empty()) {
    r1 = &data_[static_cast<uint8_t>(TemplateType::SINGLE)]
               [static_cast<uint8_t>(MappingType::PRIMARY)]
                   .front();
  } else if (!data_[static_cast<uint8_t>(TemplateType::SINGLE)]
                   [static_cast<uint8_t>(MappingType::UNMAPPED)]
                       .empty()) {
    r1 = &data_[static_cast<uint8_t>(TemplateType::SINGLE)]
               [static_cast<uint8_t>(MappingType::UNMAPPED)]
                   .front();
  } else if (!data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
                   [static_cast<uint8_t>(MappingType::PRIMARY)]
                       .empty()) {
    r1 = &data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
               [static_cast<uint8_t>(MappingType::PRIMARY)]
                   .front();
  } else if (!data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
                   [static_cast<uint8_t>(MappingType::UNMAPPED)]
                       .empty()) {
    r1 = &data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
               [static_cast<uint8_t>(MappingType::UNMAPPED)]
                   .front();
  }
  if (!data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
            [static_cast<uint8_t>(MappingType::PRIMARY)]
                .empty()) {
    r2 = &data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
               [static_cast<uint8_t>(MappingType::PRIMARY)]
                   .front();
  } else if (!data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
                   [static_cast<uint8_t>(MappingType::UNMAPPED)]
                       .empty()) {
    r2 = &data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
               [static_cast<uint8_t>(MappingType::UNMAPPED)]
                   .front();
  }

  return std::make_pair(r1, r2);
}

// -----------------------------------------------------------------------------
bool SamRecordGroup::IsR1First(
    const std::pair<bool, core::record::ClassType>& cls) const {
  if (cls.second == core::record::ClassType::kClassU || !cls.first ||
      data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
           [static_cast<uint8_t>(MappingType::PRIMARY)]
               .empty()) {
    return true;
  }
  if (data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
           [static_cast<uint8_t>(MappingType::PRIMARY)]
               .empty()) {
    return false;
  }
  return data_[static_cast<uint8_t>(TemplateType::PAIRED_1)]
              [static_cast<uint8_t>(MappingType::PRIMARY)]
                  .front()
                  .GetPos() <
         data_[static_cast<uint8_t>(TemplateType::PAIRED_2)]
              [static_cast<uint8_t>(MappingType::PRIMARY)]
                  .front()
                  .GetPos();
}

// -----------------------------------------------------------------------------
void SamRecordGroup::AddSegment(core::record::Record& rec, SamRecord* r1) {
  if (r1) {
    core::record::Segment seg(r1->MoveSeq());
    if (r1->GetQual() != "*") {
      seg.AddQualities(r1->MoveQual());
    } else {
      seg.AddQualities("");
    }
    rec.AddSegment(std::move(seg));
  }
}

// -----------------------------------------------------------------------------
void SamRecordGroup::convert(std::list<core::record::Record>& records, bool) {
  auto cls = validate();
  if (cls.second == core::record::ClassType::kNone) {
    return;
  }

  auto [fst, snd] = GetReadTuple();
  SamRecord* r1 = fst;
  SamRecord* r2 = snd;

  if (r1 && r2 && r1->GetRid() != r2->GetRid()) {
    core::record::Record rec1(2, cls.second, r1->MoveQname(), "",
                              std::get<1>(ConvertFlags2Mpeg(r1->GetFlag())),
                              true);
    core::record::Record rec2(2, cls.second, r2->MoveQname(), "",
                              std::get<1>(ConvertFlags2Mpeg(r2->GetFlag())),
                              false);

    AddSegment(rec1, r1);
    AddSegment(rec2, r2);

    AddAlignment(rec1, r1, nullptr, true);
    AddAlignment(rec2, r2, nullptr, true);

    records.push_back(std::move(rec1));
    records.push_back(std::move(rec2));
    return;
  }

  auto flag_tuple = ConvertFlags2Mpeg(r1 ? r1->GetFlag() : r2->GetFlag());
  bool is_r1_first = IsR1First(cls);

  core::record::Record rec(cls.first ? 2 : 1, cls.second,
                           r1 ? r1->MoveQname() : r2->MoveQname(), "Genie",
                           std::get<1>(flag_tuple), is_r1_first);

  AddSegment(rec, r1);
  AddSegment(rec, r2);

  if (cls.second == core::record::ClassType::kClassU) {
    records.push_back(std::move(rec));
    return;
  }

  if (is_r1_first) {
    AddAlignment(rec, r1, r2, cls.first);
  } else {
    AddAlignment(rec, r2, r1, cls.first);
  }

  if (!rec.IsRead1First()) {
    rec.SwapSegmentOrder();
  }

  records.push_back(std::move(rec));
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

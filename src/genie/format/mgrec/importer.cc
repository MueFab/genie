/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgrec/importer.h"

#include <genie/core/record/alignment_split/same_rec.h>

#include <iostream>
#include <string>
#include <utility>

#include "genie/util/log.h"
#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Mgrec";

namespace genie::format::mgrec {

// -----------------------------------------------------------------------------
Importer::Importer(const size_t block_size, std::istream& file_1,
                   std::ostream& unsupported, const bool check_support)
    : block_size_(block_size),
      reader_(file_1),
      writer_(unsupported),
      buffered_record_(std::nullopt),
      check_support_(check_support),
      file_size_(0),
      last_progress_(0) {
  const auto pos = file_1.tellg();
  file_1.seekg(0, std::ios::end);
  file_size_ = file_1.tellg();
  file_1.seekg(pos);
}

// -----------------------------------------------------------------------------
bool IsECigarSupported(const std::string& e_cigar) {
  // Splices not supported
  if (e_cigar.find_first_of('*') != std::string::npos ||
      e_cigar.find_first_of('/') != std::string::npos ||
      e_cigar.find_first_of('%') != std::string::npos) {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
bool Importer::IsRecordSupported(const core::record::Record& rec) {
  if (!check_support_) {
    return true;
  }
  /* if (rec.getClassID() == genie::core::record::ClassType::CLASS_U &&
       rec.getSegments().Size() != rec.getNumberOfTemplateSegments()) {
       discarded_missing_pair_U++;
       return false;
   }*/
  for (const auto& a : rec.GetAlignments()) {
    if (rec.GetClassId() == core::record::ClassType::kClassHm) {
      discarded_hm_++;
      return false;
    }
    if (!IsECigarSupported(a.GetAlignment().GetECigar())) {
      discarded_splices_++;
      return false;
    }
    for (const auto& s : a.GetAlignmentSplits()) {
      if (s->GetType() == core::record::AlignmentSplit::Type::kSameRec) {
        if (!IsECigarSupported(
                dynamic_cast<core::record::alignment_split::SameRec&>(*s)
                    .GetAlignment()
                    .GetECigar())) {
          discarded_splices_++;
          return false;
        }
        // Splits with more than 32767 delta must be encoded in separate
        // records, which is not yet supported
        if (std::abs(dynamic_cast<core::record::alignment_split::SameRec&>(*s)
                         .GetDelta()) > 32767) {
          discarded_long_distance_++;
          return false;
        }
      }
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
bool Importer::PumpRetrieve(core::Classifier* classifier) {
  const util::Watch watch;
  core::record::Chunk chunk;
  bool seq_id_valid = false;
  for (size_t i = 0; i < block_size_; ++i) {
    if (buffered_record_) {
      chunk.SetRefId(buffered_record_->GetAlignmentSharedData().GetSeqId());
      seq_id_valid = true;
      if (IsRecordSupported(buffered_record_.value())) {
        chunk.GetData().emplace_back(std::move(buffered_record_.value()));
      } else {
        buffered_record_.value().Write(writer_);
      }
      buffered_record_ = std::nullopt;
      continue;
    }
    core::record::Record rec(reader_);
    if (!reader_.IsStreamGood()) {
      break;
    }

    if (!seq_id_valid) {
      chunk.SetRefId(rec.GetAlignmentSharedData().GetSeqId());
      seq_id_valid = true;
    }

    if (chunk.GetRefId() != rec.GetAlignmentSharedData().GetSeqId()) {
      buffered_record_ = std::move(rec);
      break;
    }
    if (IsRecordSupported(rec)) {
      chunk.GetData().emplace_back(std::move(rec));
    } else {
      rec.Write(writer_);
    }
  }

  if (reader_.IsStreamGood()) {
    const auto progress = static_cast<float>(reader_.GetStreamPosition()) /
                          static_cast<float>(file_size_);
    while (progress - last_progress_ > 0.05) {  // NOLINT
      GENIE_LOG(
          util::Logger::Severity::INFO,
          "Progress in file: " +
              std::to_string(static_cast<int>(std::round(progress * 100))) +
              "%");
      last_progress_ += 0.05;
    }
  }

  chunk.GetStats().AddDouble("time-mgrec-import", watch.Check());
  for (const auto& c : chunk.GetData()) {
    missing_additional_alignments_ +=
        c.GetAlignments().empty() ? 0 : c.GetAlignments().size() - 1;
  }
  classifier->Add(std::move(chunk));
  return reader_.IsStreamGood() || buffered_record_;
}

// -----------------------------------------------------------------------------
void Importer::PrintStats() const {
  if (discarded_splices_ + discarded_hm_ + discarded_long_distance_ +
          discarded_missing_pair_u_ ==
      0) {
    GENIE_LOG(util::Logger::Severity::INFO, "No reads were dropped");
    return;
  }

  GENIE_LOG(util::Logger::Severity::WARNING,
            "The following number of reads were dropped:");
  GENIE_LOG(util::Logger::Severity::WARNING,
            std::to_string(discarded_splices_) + " containing splices");
  GENIE_LOG(util::Logger::Severity::WARNING,
            std::to_string(discarded_hm_) + " class HM reads");
  GENIE_LOG(util::Logger::Severity::WARNING,
            std::to_string(discarded_long_distance_) +
                " aligned, paired reads with mapping distance too big");
  GENIE_LOG(util::Logger::Severity::WARNING,
            std::to_string(discarded_missing_pair_u_) +
                " unaligned reads with missing pair");
  GENIE_LOG(
      util::Logger::Severity::WARNING,
      std::to_string(discarded_splices_ + discarded_hm_ +
                     discarded_long_distance_ + discarded_missing_pair_u_) +
          " in total");
  GENIE_LOG(util::Logger::Severity::WARNING,
            std::to_string(missing_additional_alignments_) +
                " additional alignments were dropped");
}

// -----------------------------------------------------------------------------
void Importer::FlushIn(uint64_t& pos) {
  FormatImporter::FlushIn(pos);
  PrintStats();
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

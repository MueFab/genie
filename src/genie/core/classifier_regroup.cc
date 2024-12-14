/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/classifier_regroup.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/log.h"
#include "record/alignment_split/same_rec.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Classifier";

namespace genie::core {

// -----------------------------------------------------------------------------
bool ClassifierRegroup::IsCovered(const size_t start, const size_t end) const {
  size_t position = start;

  for (auto it = current_seq_coverage_.begin();
       it != current_seq_coverage_.end(); ++it) {
    if (position >= it->first && position < it->second) {
      position = it->second - 1;
      it = current_seq_coverage_.begin();
    }
    if (position >= end - 1) {
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------

bool ClassifierRegroup::IsCovered(const record::Record& r) const {
  for (size_t i = 0;
       i < r.GetAlignments().front().GetAlignmentSplits().size() + 1; ++i) {
    if (i > 0 &&
        r.GetAlignments().front().GetAlignmentSplits()[i - 1]->GetType() !=
            record::AlignmentSplit::Type::kSameRec) {
      continue;
    }
    if (const auto pos = r.GetPosition(0, i);
        !IsCovered(pos, pos + r.GetMappedLength(0, i))) {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------

void ClassifierRegroup::QueueFinishedChunk(record::Chunk& data) {
  if (!data.GetRef().IsEmpty()) {
    if (ref_mode_ == RefMode::kRelevant) {
      for (size_t i = data.GetRef().GetGlobalStart() /
                      ReferenceManager::GetChunkSize();
           i <= (data.GetRef().GetGlobalEnd() - 1) /
                    ReferenceManager::GetChunkSize();
           ++i) {
        if (IsWritten(data.GetRef().GetRefName(), i)) {
          continue;
        }
        ref_state_.at(data.GetRef().GetRefName()).at(i) = 1;

        if (raw_ref_mode_) {
          size_t length = ref_mgr_->GetLength(data.GetRef().GetRefName());
          data.AddRefToWrite(
              i * ReferenceManager::GetChunkSize(),
              std::min((i + 1) * ReferenceManager::GetChunkSize(), length));
        } else {
          size_t length = ref_mgr_->GetLength(data.GetRef().GetRefName());
          record::Chunk ref_chunk;
          ref_chunk.SetReferenceOnly(true);
          ref_chunk.SetRefId(ref_mgr_->Ref2Id(data.GetRef().GetRefName()));
          ref_chunk.GetRef() = data.GetRef();
          record::Record rec(1, record::ClassType::kClassU, "", "", 0);
          std::string seq =
              *data.GetRef().GetChunkAt(i * ReferenceManager::GetChunkSize());
          if ((i + 1) * ReferenceManager::GetChunkSize() > length) {
            seq = seq.substr(0, length - i * ReferenceManager::GetChunkSize());
          }
          record::Segment segment(std::move(seq));
          rec.AddSegment(std::move(segment));
          ref_chunk.GetData().push_back(std::move(rec));
          finished_chunks_.push_back(std::move(ref_chunk));
        }
      }
    }

    data.SetRefId(ref_mgr_->Ref2Id(data.GetRef().GetRefName()));
  }
  finished_chunks_.push_back(std::move(data));
}

// -----------------------------------------------------------------------------

bool ClassifierRegroup::IsWritten(const std::string& ref, const size_t index) {
  if (ref_state_.find(ref) == ref_state_.end()) {
    ref_state_.insert(std::make_pair(ref, std::vector<uint8_t>(1, 0)));
  }
  if (ref_state_.at(ref).size() <= index) {
    ref_state_.at(ref).resize(index + 1, 0);
  }
  return ref_state_.at(ref).at(index);
}

// -----------------------------------------------------------------------------

ClassifierRegroup::ClassifierRegroup(const size_t au_size,
                                     ReferenceManager* ref_manager,
                                     const RefMode mode, const bool raw_ref)
    : ref_mgr_(ref_manager),
      current_seq_id_(-1),
      au_size_(au_size),
      ref_mode_(mode),
      raw_ref_mode_(raw_ref) {
  current_chunks_.resize(2);
  for (auto& c : current_chunks_) {
    c.resize(2);
    for (auto& c2 : c) {
      c2.resize(static_cast<uint8_t>(record::ClassType::kClassU));
    }
  }
}

// -----------------------------------------------------------------------------

record::Chunk ClassifierRegroup::GetChunk() {
  if (ref_mode_ == RefMode::kFull) {
    auto seq_vec = ref_mgr_->GetSequences();
    while (true) {
      if (ref_mode_full_seq_id_ != seq_vec.size()) {
        const auto& seq = seq_vec.at(ref_mode_full_seq_id_);
        auto cov_vec = ref_mgr_->GetCoverage(seq);
        auto [fst, snd] = cov_vec.at(ref_mode_full_cov_id_);

        if (size_t chunk_offset = fst / ReferenceManager::GetChunkSize();
            !IsWritten(seq, chunk_offset + ref_mode_full_chunk_id_)) {
          record::Chunk ref_chunk;

          GENIE_LOG(util::Logger::Severity::INFO,
                    "Writing ref " + seq + " [" +
                        std::to_string(std::max(
                            fst, (chunk_offset + ref_mode_full_chunk_id_) *
                                     ReferenceManager::GetChunkSize())) +
                        ", " +
                        std::to_string(std::min(
                            snd, (chunk_offset + ref_mode_full_chunk_id_ + 1) *
                                     ReferenceManager::GetChunkSize())) +
                        "]");

          if (raw_ref_mode_) {
            ref_chunk.AddRefToWrite(
                std::max(fst, (chunk_offset + ref_mode_full_chunk_id_) *
                                  ReferenceManager::GetChunkSize()),
                std::min(snd, (chunk_offset + ref_mode_full_chunk_id_ + 1) *
                                  ReferenceManager::GetChunkSize()));
            ref_chunk.GetRef() = ref_mgr_->Load(
                seq,
                std::max(fst, (chunk_offset + ref_mode_full_chunk_id_) *
                                  ReferenceManager::GetChunkSize()),
                std::min(snd, (chunk_offset + ref_mode_full_chunk_id_ + 1) *
                                  ReferenceManager::GetChunkSize()));
            ref_chunk.SetRefId(ref_mgr_->Ref2Id(seq));
          } else {
            ref_chunk.SetReferenceOnly(true);
            ref_chunk.SetRefId(ref_mgr_->Ref2Id(seq));
            ref_chunk.GetRef() = ref_mgr_->Load(
                seq,
                std::max(fst, (chunk_offset + ref_mode_full_chunk_id_) *
                                  ReferenceManager::GetChunkSize()),
                std::min(snd, (chunk_offset + ref_mode_full_chunk_id_ + 1) *
                                  ReferenceManager::GetChunkSize()));
            record::Record rec(1, record::ClassType::kClassU, "", "", 0);
            std::string ref_seq = *ref_chunk.GetRef().GetChunkAt(
                (chunk_offset + ref_mode_full_chunk_id_) *
                ReferenceManager::GetChunkSize());
            if (ref_seq.empty()) {
              GENIE_LOG(util::Logger::Severity::WARNING,
                        "Empty ref sequence for " + seq);
            }
            if (fst > (chunk_offset + ref_mode_full_chunk_id_) *
                          ReferenceManager::GetChunkSize()) {
              ref_seq = ref_seq.substr(
                  fst - (chunk_offset + ref_mode_full_chunk_id_) *
                            ReferenceManager::GetChunkSize());
            }
            if (snd < (chunk_offset + ref_mode_full_chunk_id_ + 1) *
                          ReferenceManager::GetChunkSize()) {
              ref_seq =
                  ref_seq.substr(0, (chunk_offset + ref_mode_full_chunk_id_) *
                                            ReferenceManager::GetChunkSize() -
                                        snd);
            }
            record::Segment segment(std::move(ref_seq));
            rec.AddSegment(std::move(segment));
            ref_chunk.GetData().push_back(std::move(rec));
          }

          ref_mode_full_chunk_id_++;
          if (ref_mode_full_chunk_id_ >
              (snd - 1) / ReferenceManager::GetChunkSize()) {
            ref_mode_full_cov_id_++;
            ref_mode_full_chunk_id_ = 0;
          }
          if (ref_mode_full_cov_id_ == cov_vec.size()) {
            ref_mode_full_cov_id_ = 0;
            ref_mode_full_seq_id_++;
          }

          return ref_chunk;
        }
        ref_mode_full_chunk_id_++;
        if (ref_mode_full_chunk_id_ >
            (snd - 1) / ReferenceManager::GetChunkSize()) {
          ref_mode_full_cov_id_++;
          ref_mode_full_chunk_id_ = 0;
        }
        if (ref_mode_full_cov_id_ == cov_vec.size()) {
          ref_mode_full_cov_id_ = 0;
          ref_mode_full_seq_id_++;
        }
      } else {
        break;
      }
    }
  }

  auto ret = record::Chunk();
  if (finished_chunks_.empty()) {
    return ret;
  }

  ret = std::move(finished_chunks_.front());
  finished_chunks_.erase(finished_chunks_.begin());

#define AU_DEBUG_WRITE 0
#if AU_DEBUG_WRITE
  static int i = 0;
  std::ofstream tmpOut("AU_" + std::to_string(i++) + ".mgrec");
  util::BitWriter bw(&tmpOut);
  for (const auto& r : ret.getData()) {
    r.write(bw);
  }
#endif
  return ret;
}

// -----------------------------------------------------------------------------

void ClassifierRegroup::Add(record::Chunk&& c) {
  record::Chunk chunk = std::move(c);
  bool moved_stats = false;

  // New reference
  if (chunk.GetRefId() != current_seq_id_) {
    current_seq_id_ = static_cast<uint16_t>(chunk.GetRefId());
    if (ref_mgr_->RefKnown(current_seq_id_)) {
      current_seq_coverage_ =
          ref_mgr_->GetCoverage(ref_mgr_->Id2Ref(current_seq_id_));
    } else {
      current_seq_coverage_ = std::vector<std::pair<size_t, size_t>>();
    }
    for (auto& ref_block : current_chunks_) {
      for (auto& pair_block : ref_block) {
        for (auto& class_block : pair_block) {
          if (class_block.GetData().empty() ||
              class_block.GetData().front().GetClassId() ==
                  record::ClassType::kClassU) {
            continue;
          }
          QueueFinishedChunk(class_block);
        }
      }
    }
  }

  for (auto& r : chunk.GetData()) {
    auto class_type = r.GetClassId();  // Only look at the e_cigar for first
                                       // classification
    const bool paired = r.GetNumberOfTemplateSegments() > 1;
    bool ref_based = false;

    if (r.GetClassId() == record::ClassType::kClassU &&
        r.GetNumberOfTemplateSegments() != r.GetSegments().size()) {
      current_unpaired_u_chunk_.GetData().emplace_back(std::move(r));
      if (current_unpaired_u_chunk_.GetData().size() >= au_size_) {
        QueueFinishedChunk(current_unpaired_u_chunk_);
      }
      continue;
    }

    ReferenceManager::ReferenceExcerpt record_reference;

    // Unaligned reads can't be ref based, otherwise check if reference
    // available
    if (class_type != record::ClassType::kClassU) {
      ref_based = IsCovered(r);
      if (ref_based) {
        size_t end = 0;
        if (r.GetAlignments().front().GetAlignmentSplits().empty() ||
            r.GetAlignments().front().GetAlignmentSplits().front()->GetType() !=
                record::AlignmentSplit::Type::kSameRec) {
          end =
              r.GetAlignments().front().GetPosition() + r.GetMappedLength(0, 0);
        } else {
          end = r.GetAlignments().front().GetPosition() +
                r.GetMappedLength(0, 1) +
                dynamic_cast<const record::alignment_split::SameRec&>(
                    *r.GetAlignments().front().GetAlignmentSplits().front())
                    .GetDelta();
          end = std::max(end, r.GetAlignments().front().GetPosition() +
                                  r.GetMappedLength(0, 0));
        }
        record_reference = this->ref_mgr_->Load(
            ref_mgr_->Id2Ref(r.GetAlignmentSharedData().GetSeqId()),
            r.GetAlignments().front().GetPosition(), end);
      }
    }

    if (current_chunks_[ref_based][paired][static_cast<uint8_t>(class_type) - 1]
            .GetData()
            .empty()) {
      current_chunks_[ref_based][paired][static_cast<uint8_t>(class_type) - 1]
          .GetRef() = record_reference;
    } else {
      current_chunks_[ref_based][paired][static_cast<uint8_t>(class_type) - 1]
          .GetRef()
          .Merge(record_reference);
    }
    if (!moved_stats) {
      current_chunks_[ref_based][paired][static_cast<uint8_t>(class_type) - 1]
          .GetStats()
          .Add(chunk.GetStats());
      moved_stats = true;
    }
    current_chunks_[ref_based][paired][static_cast<uint8_t>(class_type) - 1]
        .GetData()
        .push_back(r);
    if (current_chunks_[ref_based][paired][static_cast<uint8_t>(class_type) - 1]
            .GetData()
            .size() == au_size_) {
      auto& class_block = current_chunks_[ref_based][paired]
                                         [static_cast<uint8_t>(class_type) - 1];
      QueueFinishedChunk(class_block);
    }
  }
}

// -----------------------------------------------------------------------------

void ClassifierRegroup::Flush() {
  for (auto& ref_block : current_chunks_) {
    for (auto& pair_block : ref_block) {
      for (auto& class_block : pair_block) {
        if (class_block.GetData().empty()) {
          continue;
        }
        QueueFinishedChunk(class_block);
      }
    }
  }
  if (!current_unpaired_u_chunk_.GetData().empty()) {
    QueueFinishedChunk(current_unpaired_u_chunk_);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

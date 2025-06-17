/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord::ContactRecord(uint16_t _sample_ID, std::string&& _sample_name, uint32_t _bin_size,
                             uint8_t _chr1_ID, std::string&& _chr1_name, uint64_t _chr1_length,
                             uint8_t _chr2_ID, std::string&& _chr2_name, uint64_t _chr2_length,
                             std::vector<std::string>&& _norm_count_names,
                             std::vector<uint64_t>&& _start_pos1, std::vector<uint64_t>&& _end_pos1,
                             std::vector<uint64_t>&& _start_pos2, std::vector<uint64_t>&& _end_pos2,
                             std::vector<uint32_t>&& _counts)
    : sample_id_(_sample_ID),
      sample_name_(std::move(_sample_name)),
      bin_size_(_bin_size),
      chr1_id_(_chr1_ID),
      chr1_name_(std::move(_chr1_name)),
      chr1_length_(_chr1_length),
      chr2_id_(_chr2_ID),
      chr2_name_(std::move(_chr2_name)),
      chr2_length_(_chr2_length),
      norm_count_names_(std::move(_norm_count_names)),
      start_pos1_(std::move(_start_pos1)),
      end_pos1_(std::move(_end_pos1)),
      start_pos2_(std::move(_start_pos2)),
      end_pos2_(std::move(_end_pos2)),
      counts_(std::move(_counts)) {}

// ---------------------------------------------------------------------------------------------------------------------

// ContactRecord::ContactRecord(const ContactRecord& rec) {*this = rec;}

// ---------------------------------------------------------------------------------------------------------------------

// ContactRecord::ContactRecord(ContactRecord&& rec) noexcept {*this = std::move(rec); }

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord::ContactRecord(util::BitReader& reader) {
  // Sample
  sample_id_ = reader.ReadAlignedInt<uint16_t>();
  sample_name_.resize(reader.ReadAlignedInt<uint8_t>());
  reader.ReadAlignedBytes(&sample_name_[0], sample_name_.size());
  bin_size_ = reader.ReadAlignedInt<uint32_t>();

  // chr1
  chr1_id_ = reader.ReadAlignedInt<uint8_t>();
  chr1_name_.resize(reader.ReadAlignedInt<uint8_t>());
  reader.ReadAlignedBytes(&chr1_name_[0], chr1_name_.size());
  chr1_length_ = reader.Read<uint64_t>();

  // chr2
  chr2_id_ = reader.ReadAlignedInt<uint8_t>();
  chr2_name_.resize(reader.ReadAlignedInt<uint8_t>());
  reader.ReadAlignedBytes(&chr2_name_[0], chr2_name_.size());
  chr2_length_ = reader.Read<uint64_t>();

  // num_entries and num_norm_counts
  auto num_entries = reader.ReadAlignedInt<uint64_t>();
  auto num_norm_counts = reader.ReadAlignedInt<uint8_t>();

  norm_count_names_.resize(num_norm_counts);
  for (auto i = 0; i < num_norm_counts; i++) {
    norm_count_names_[i].resize(reader.ReadAlignedInt<uint8_t>());
    reader.ReadAlignedBytes(&norm_count_names_[i][0], norm_count_names_[i].size());
  }

  start_pos1_.resize(num_entries);
  for (size_t i = 0; i < num_entries; i++) {
    start_pos1_[i] = reader.ReadAlignedInt<uint64_t>();
  }

  end_pos1_.resize(num_entries);
  for (size_t i = 0; i < num_entries; i++) {
    end_pos1_[i] = reader.ReadAlignedInt<uint64_t>();
  }

  start_pos2_.resize(num_entries);
  for (size_t i = 0; i < num_entries; i++) {
    start_pos2_[i] = reader.ReadAlignedInt<uint64_t>();
  }

  end_pos2_.resize(num_entries);
  for (size_t i = 0; i < num_entries; i++) {
    end_pos2_[i] = reader.ReadAlignedInt<uint64_t>();
  }

  counts_.resize(num_entries);
  for (size_t i = 0; i < num_entries; i++) {
    counts_[i] = reader.ReadAlignedInt<uint32_t>();
  }

  norm_counts_.resize(num_norm_counts);
  for (size_t j = 0; j < num_norm_counts; j++) {
    norm_counts_[j].resize(num_entries);
    for (size_t i = 0; i < num_entries; i++) {
      auto tmp_val = reader.ReadAlignedInt<uint64_t>();
      double norm_count;
      std::memcpy(&norm_count, &tmp_val, sizeof(norm_count));
      norm_counts_[j][i] = norm_count;
    }
  }

  auto link_record_flag = reader.ReadAlignedInt<uint8_t>();
  UTILS_DIE_IF(link_record_flag, "Not yet implemented for link_record!");
}

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord& ContactRecord::operator=(const ContactRecord& rec) {
  if (this == &rec) {
    return *this;
  }

  this->sample_id_ = rec.sample_id_;
  this->sample_name_ = rec.sample_name_;
  this->bin_size_ = rec.bin_size_;
  this->chr1_id_ = rec.chr1_id_;
  this->chr1_name_ = rec.chr1_name_;
  this->chr1_length_ = rec.chr1_length_;
  this->chr2_id_ = rec.chr2_id_;
  this->chr2_name_ = rec.chr2_name_;
  this->chr2_length_ = rec.chr2_length_;
  this->norm_count_names_ = rec.norm_count_names_;
  this->start_pos1_ = rec.start_pos1_;
  this->end_pos1_ = rec.end_pos1_;
  this->start_pos2_ = rec.start_pos2_;
  this->end_pos2_ = rec.end_pos2_;
  this->counts_ = rec.counts_;

  return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord& ContactRecord::operator=(ContactRecord&& rec) noexcept {
  this->sample_id_ = rec.sample_id_;
  this->sample_name_ = std::move(rec.sample_name_);
  this->bin_size_ = rec.bin_size_;
  this->chr1_id_ = rec.chr1_id_;
  this->chr1_name_ = std::move(rec.chr1_name_);
  this->chr1_length_ = rec.chr1_length_;
  this->chr2_id_ = rec.chr2_id_;
  this->chr2_name_ = std::move(rec.chr2_name_);
  this->chr2_length_ = rec.chr2_length_;
  this->norm_count_names_ = std::move(rec.norm_count_names_);
  this->start_pos1_ = std::move(rec.start_pos1_);
  this->end_pos1_ = std::move(rec.end_pos1_);
  this->start_pos2_ = std::move(rec.start_pos2_);
  this->end_pos2_ = std::move(rec.end_pos2_);
  this->counts_ = std::move(rec.counts_);

  return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ContactRecord::operator==(const ContactRecord& other) {
  bool ret = sample_id_ == other.sample_id_ && sample_name_ == other.sample_name_ &&
             bin_size_ == other.bin_size_ && chr1_id_ == other.chr1_id_ &&
             chr1_name_ == other.chr1_name_ && chr1_length_ == other.chr1_length_ &&
             chr2_id_ == other.chr2_id_ && chr2_name_ == other.chr2_name_ &&
             chr2_length_ == other.chr2_length_ &&
             //        norm_count_names.size() == other.norm_count_names.size() &&
             norm_count_names_ == other.norm_count_names_ &&
             //        start_pos1.size() == other.start_pos1.size() &&
             start_pos1_ == other.start_pos1_ &&
             //        end_pos1.size() == other.end_pos1.size() &&
             end_pos1_ == other.end_pos1_ &&
             //        start_pos2.size() == other.start_pos2.size() &&
             start_pos2_ == other.start_pos2_ &&
             //        end_pos2.size() == other.end_pos2.size() &&
             end_pos2_ == other.end_pos2_ &&
             //        counts.size() == other.counts.size() &&
             counts_ == other.counts_ &&
             //        norm_counts.size() == other.norm_counts.size() &&
             norm_counts_ == other.norm_counts_ &&
             link_record_.has_value() == other.link_record_.has_value();

  // TODO(yeremia): this this "const" problem
  //    if (ret){
  //        const auto& this_link_record = link_record.value();
  //        const auto& other_link_record = other.link_record.value();
  //        ret = this_link_record == other_link_record;
  //    }

  return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::TransposeCM() {
  uint8_t chr_ID = chr1_id_;
  std::string chr_name = std::move(chr1_name_);
  uint64_t chr_length = chr1_length_;

  chr1_id_ = chr2_id_;
  chr1_name_ = std::move(chr2_name_);
  chr1_length_ = chr2_length_;
  chr2_id_ = chr_ID;
  chr2_name_ = std::move(chr_name);
  chr2_length_ = chr_length;

  std::vector<uint64_t> start_pos = std::move(start_pos1_);
  std::vector<uint64_t> end_pos = std::move(end_pos1_);
  start_pos1_ = std::move(start_pos2_);
  end_pos1_ = std::move(end_pos2_);
  start_pos2_ = std::move(start_pos1_);
  end_pos2_ = std::move(end_pos1_);
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t ContactRecord::GetSampleID() const {
  return sample_id_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::GetSampleName() const {
  return sample_name_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactRecord::GetBinSize() const {
  return bin_size_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::GetChr1ID() const {
  return chr1_id_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::GetChr1Name() const {
  return chr1_name_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::GetChr1Length() const {
  return chr1_length_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::GetChr2ID() const {
  return chr2_id_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::GetChr2Name() const {
  return chr2_name_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::GetChr2Length() const {
  return chr2_length_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::GetNumEntries() const {
  return counts_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::GetNumNormCounts() const {
  return static_cast<uint8_t>(norm_count_names_.size());
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& ContactRecord::GetNormCountNames() const {
  return norm_count_names_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::GetStartPos1() const {
  return start_pos1_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::GetEndPos1() const {
  return end_pos1_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::GetStartPos2() const {
  return start_pos2_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::GetEndPos2() const {
  return end_pos2_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint32_t>& ContactRecord::GetCounts() const {
  return counts_;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<double_t>>& ContactRecord::GetNormCounts() const {
  return norm_counts_;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetSampleId(uint16_t _sample_ID) {
  sample_id_ = _sample_ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetSampleName(std::string&& _sample_name) {
  sample_name_ = std::move(_sample_name);
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetBinSize(uint32_t _bin_size) {
  bin_size_ = _bin_size;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetChr1ID(uint8_t _chr1_ID) {
  chr1_id_ = _chr1_ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetChr1Name(std::string&& _chr1_name) {
  chr1_name_ = std::move(_chr1_name);
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetChr1Length(uint64_t _chr1_len) {
  chr1_length_ = _chr1_len;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetChr2ID(uint8_t _chr2_ID) {
  chr2_id_ = _chr2_ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetChr2Name(std::string&& _chr2_name) {
  chr2_name_ = std::move(_chr2_name);
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetChr2Length(uint64_t _chr2_len) {
  chr2_length_ = _chr2_len;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetCMValues(std::vector<uint64_t>&& start_pos1,
                                std::vector<uint64_t>&& end_pos1,
                                std::vector<uint64_t>&& start_pos2,
                                std::vector<uint64_t>&& end_pos2, std::vector<uint32_t>&& counts) {
  UTILS_DIE_IF(counts.size() != start_pos1.size() || counts.size() != end_pos1.size() ||
                   counts.size() != start_pos2.size() || counts.size() != end_pos2.size(),
               "Invalid length of CM values");

  start_pos1_ = std::move(start_pos1);
  end_pos1_ = std::move(end_pos1);
  start_pos2_ = std::move(start_pos2);
  end_pos2_ = std::move(end_pos2);
  counts_ = std::move(counts);
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): Implement this
void ContactRecord::Write(util::BitWriter& writer) const {
  // Sample
  writer.WriteBypassBE(sample_id_);
  writer.WriteBypassBE(static_cast<uint8_t>(sample_name_.length()));
  writer.WriteAlignedBytes(sample_name_.data(), sample_name_.length());
  writer.WriteBypassBE(bin_size_);

  // chr1
  writer.WriteBypassBE(chr1_id_);
  writer.WriteBypassBE(static_cast<uint8_t>(chr1_name_.length()));
  writer.WriteAlignedBytes(chr1_name_.data(), chr1_name_.length());
  writer.WriteBypassBE(chr1_length_);

  // chr2
  writer.WriteBypassBE(chr2_id_);
  writer.WriteBypassBE(static_cast<uint8_t>(chr2_name_.length()));
  writer.WriteAlignedBytes(chr2_name_.data(), chr2_name_.length());
  writer.WriteBypassBE(chr2_length_);

  // num_entries and num_norm_counts
  writer.WriteBypassBE(GetNumEntries());
  writer.WriteBypassBE(GetNumNormCounts());

  for (auto i = 0; i < GetNumNormCounts(); i++) {
    writer.WriteBypassBE(static_cast<uint8_t>(norm_count_names_[i].length()));
    writer.WriteAlignedBytes(norm_count_names_[i].data(), norm_count_names_[i].length());
  }

  for (const auto& v : start_pos1_) {
    writer.WriteBypassBE(v);
  }

  for (const auto& v : end_pos1_) {
    writer.WriteBypassBE(v);
  }

  for (const auto& v : start_pos2_) {
    writer.WriteBypassBE(v);
  }

  for (const auto& v : end_pos2_) {
    writer.WriteBypassBE(v);
  }

  for (const auto& v : counts_) {
    writer.WriteBypassBE(v);
  }

  for (const auto& norm_count_vals : norm_counts_) {
    for (const auto& v : norm_count_vals) {
      writer.WriteBypassBE(v);
    }
  }

  // TODO(yeremia): fix the flag
  //    auto link_record_flag = reader.ReadAlignedInt<uint8_t>();
  writer.WriteBypassBE(static_cast<uint8_t>(0));
  //    UTILS_DIE_IF(link_record_flag, "Not yet implemented for link_record!");
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

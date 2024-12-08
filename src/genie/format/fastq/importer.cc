/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fastq/importer.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/core/record/class_type.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::format::fastq {

// -----------------------------------------------------------------------------
Importer::Importer(const size_t block_size, std::istream& file_1)
    : block_size_(block_size), file_list_{&file_1} {}

// -----------------------------------------------------------------------------
Importer::Importer(const size_t block_size, std::istream& file_1,
                   std::istream& file_2)
    : block_size_(block_size), file_list_{&file_1, &file_2} {}

// -----------------------------------------------------------------------------
bool Importer::PumpRetrieve(core::Classifier* classifier) {
  util::Watch watch;
  core::record::Chunk chunk;
  size_t size_seq = 0;
  size_t size_quality = 0;
  size_t size_name = 0;
  bool eof = false;
  {
    for (size_t cur_record = 0; cur_record < block_size_; ++cur_record) {
      auto data = ReadData(file_list_);
      if (data.empty()) {
        eof = true;
        break;
      }
      auto record = BuildRecord(data);
      for (const auto& seg : record.GetSegments()) {
        size_seq += seg.GetSequence().size();
        for (const auto& q : seg.GetQualities()) {
          size_quality += q.size();
        }
      }
      size_name += record.GetName().size() * 2;
      chunk.GetData().push_back(std::move(record));
    }
  }

  chunk.GetStats().AddInteger("Size-fastq-sequence",
                              static_cast<int64_t>(size_seq));
  chunk.GetStats().AddInteger("Size-fastq-quality",
                              static_cast<int64_t>(size_quality));
  chunk.GetStats().AddInteger("Size-fastq-name",
                              static_cast<int64_t>(size_name));
  chunk.GetStats().AddInteger(
      "Size-fastq-total",
      static_cast<int64_t>(size_name + size_quality + size_seq));
  chunk.GetStats().AddDouble("time-fastq-import", watch.Check());
  classifier->Add(std::move(chunk));
  return !eof;
}

// -----------------------------------------------------------------------------
core::record::Record Importer::BuildRecord(
    std::vector<std::array<std::string, kLinesPerRecord>> data) {
  auto ret = core::record::Record(static_cast<uint8_t>(data.size()),
                                  core::record::ClassType::kClassU,
                                  data[FIRST][ID].substr(1), "", 0);

  for (auto& cur_rec : data) {
    auto seg = core::record::Segment(std::move(cur_rec[SEQUENCE]));
    if (!cur_rec[QUALITY].empty()) {
      seg.AddQualities(std::move(cur_rec[QUALITY]));
    }
    ret.AddSegment(std::move(seg));
  }
  return ret;
}

// -----------------------------------------------------------------------------
std::vector<std::array<std::string, kLinesPerRecord>> Importer::ReadData(
    const std::vector<std::istream*>& file_list) {
  std::vector<std::array<std::string, kLinesPerRecord>> data(file_list.size());
  for (size_t cur_file = 0; cur_file < file_list.size(); ++cur_file) {
    for (size_t cur_line = 0; cur_line < kLinesPerRecord; ++cur_line) {
      if (!std::getline(*file_list[cur_file], data[cur_file][cur_line])) {
        data.clear();
        return data;
      }
    }

    SanityCheck(data[cur_file]);
  }
  return data;
}

// -----------------------------------------------------------------------------
void Importer::SanityCheck(
    const std::array<std::string, kLinesPerRecord>& data) {
  constexpr char id_token = '@';
  UTILS_DIE_IF(data[Lines::ID].front() != id_token, "Invalid fastq identifier");
  constexpr char reserved_token = '+';
  UTILS_DIE_IF(data[Lines::RESERVED].front() != reserved_token,
               "Invalid fastq line 3");
  UTILS_DIE_IF(data[Lines::SEQUENCE].size() != data[Lines::QUALITY].size(),
               "Qualities and Sequence in fastq do not match in length");
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fastq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

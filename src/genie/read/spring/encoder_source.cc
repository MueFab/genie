/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder_source.cc
 * @brief Implementation of the SpringSource class for managing encoded read
 * sequences and access units.
 *
 * This file contains the implementation of the SpringSource class, which
 * handles the generation and encoding of read sequences into access units for
 * the Spring module. It includes functions for reading preprocessed files,
 * organizing them into access units, and managing performance statistics.
 *
 * @details The SpringSource class reads temporary files generated during
 * preprocessing, organizes them into access units, and handles the encoding of
 * read sequences according to the provided parameters. It ensures that the
 * encoding process follows the constraints defined in the compression
 * parameters and tracks performance statistics.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/spring/encoder_source.h"

#include <filesystem>  // NOLINT
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------

SpringSource::SpringSource(const std::string& temp_dir,
                           const CompressionParams& cp,
                           std::vector<core::parameter::EncodingSet>& p,
                           core::stats::PerfStats s)
    : num_a_us_(0), params_(p), stats_(std::move(s)) {
  au_id_ = 0;
  // read info about number of blocks (AUs) and the number of reads and
  // records in those
  const std::string block_info_file = temp_dir + "/block_info.bin";
  std::ifstream f_block_info(block_info_file, std::ios::binary);
  UTILS_DIE_IF(!f_block_info, "Cannot open file to read: " + block_info_file);
  f_block_info.read(reinterpret_cast<char*>(&num_a_us_), sizeof(uint32_t));
  num_reads_per_au_ = std::vector<uint32_t>(num_a_us_);
  num_records_per_au_ = std::vector<uint32_t>(num_a_us_);
  f_block_info.read(reinterpret_cast<char*>(&num_reads_per_au_[0]),
                    static_cast<std::streamsize>(num_a_us_ * sizeof(uint32_t)));
  if (!cp.paired_end)  // num reads same as num records per AU
    num_records_per_au_ = num_reads_per_au_;
  else
    f_block_info.read(
        reinterpret_cast<char*>(&num_records_per_au_[0]),
        static_cast<std::streamsize>(num_a_us_ * sizeof(uint32_t)));

  f_block_info.close();
  std::filesystem::remove(block_info_file);

  // define descriptors corresponding to reads, ids and quality (to read
  // them from file)
  read_desc_prefix_ = temp_dir + "/read_streams.";
  id_desc_prefix_ = temp_dir + "/id_streams.";
  quality_desc_prefix_ = temp_dir + "/quality_streams.";
}

// -----------------------------------------------------------------------------

bool SpringSource::Pump(uint64_t& id, std::mutex& lock) {
  core::AccessUnit au(core::parameter::EncodingSet(), 0);
  util::Section sec{};  // NOLINT
  {
    std::unique_lock guard(lock);

    if (au_id_ >= num_a_us_) {
      return false;
    }
    au =
        core::AccessUnit(std::move(params_[au_id_]), num_reads_per_au_[au_id_]);
    au.SetClassType(core::record::ClassType::kClassU);

    for (auto& d : au) {
      std::string filename;
      size_t count = GetDescriptor(d.GetId()).sub_seqs.size();
      if (d.GetId() == core::GenDesc::kReadName) {
        filename = id_desc_prefix_ + std::to_string(au_id_);
      } else if (d.GetId() == core::GenDesc::kQv) {
        filename = quality_desc_prefix_ + std::to_string(au_id_);
        count = au.GetParameters()
                    .GetQvConfig(core::record::ClassType::kClassU)
                    .GetNumSubsequences();
      } else {
        filename = read_desc_prefix_ + std::to_string(au_id_) + "." +
                   std::to_string(static_cast<uint8_t>(d.GetId()));
      }
      if (!std::filesystem::exists(filename)) {
        continue;
      }
      if (!std::filesystem::file_size(filename)) {
        std::filesystem::remove(filename);
        continue;
      }
      std::ifstream input(filename, std::ios::binary);
      UTILS_DIE_IF(!input, "Cannot open file to read: " + filename);
      util::BitReader br(input);
      d = core::AccessUnit::Descriptor(
          d.GetId(), count, std::filesystem::file_size(filename), br);
      input.close();
      std::filesystem::remove(filename);
    }
    au_id_++;
    sec = {id, au.GetNumReads(), true};
    id += sec.length;
  }
  au.SetStats(std::move(stats_));
  stats_ = core::stats::PerfStats();
  FlowOut(std::move(au), sec);
  return true;
}

// -----------------------------------------------------------------------------

void SpringSource::FlushIn(uint64_t& pos) { FlushOut(pos); }

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

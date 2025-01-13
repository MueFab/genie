/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 * @brief Implementation of Zstd-based entropy decoding for Genie.
 *
 * Provides functionality to decompress subsequences and process descriptors
 * using the Zstd library, ensuring efficient entropy decoding with performance
 * tracking.
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/zstd/decoder.h"

#include <zstd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "genie/util/runtime_exception.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::zstd {

// -----------------------------------------------------------------------------

core::AccessUnit::Subsequence decompress(core::AccessUnit::Subsequence&& data) {
  const auto id = data.GetId();

  uint8_t bytes = core::Range2Bytes(GetSubsequence(id).range);
  if (id.first == core::GenDesc::kReadName) {
    bytes = 1;
  }
  util::DataBlock in = data.Move();
  const size_t original_size =
      ZSTD_getFrameContentSize(in.GetData(), in.GetRawSize());

  UTILS_DIE_IF(original_size == ZSTD_CONTENTSIZE_ERROR,
               "ZSTD_getFrameContentSize failed");
  UTILS_DIE_IF(original_size == ZSTD_CONTENTSIZE_UNKNOWN,
               "ZSTD_getFrameContentSize unknown");

  util::DataBlock out(original_size, bytes);

  const size_t decompressed_size = ZSTD_decompress(
      out.GetData(), out.GetRawSize(), in.GetData(), in.GetRawSize());

  UTILS_DIE_IF(ZSTD_isError(decompressed_size),
               "ZSTD decompression failed: " +
                   std::string(ZSTD_getErrorName(decompressed_size)));

  return {std::move(out), data.GetId()};
}

// -----------------------------------------------------------------------------

std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats>
Decoder::Process(const core::parameter::DescriptorSubSequenceCfg& param,
                 core::AccessUnit::Descriptor& d, const bool mm_coder_enabled) {
  (void)param;
  (void)mm_coder_enabled;
  const util::Watch watch;
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> desc;
  std::get<0>(desc) = std::move(d);
  for (auto& sub_sequence : std::get<0>(desc)) {
    if (sub_sequence.IsEmpty()) {
      continue;
    }
    const auto [fst, snd] = sub_sequence.GetId();

    auto sub_seq_name = std::string();
    if (GetDescriptor(std::get<0>(desc).GetId()).token_type) {
      sub_seq_name = GetDescriptor(std::get<0>(desc).GetId()).name;
    } else {
      sub_seq_name =
          GetDescriptor(std::get<0>(desc).GetId()).name + "-" +
          GetDescriptor(std::get<0>(desc).GetId()).sub_seqs[snd].name;
    }
    if (!sub_sequence.IsEmpty()) {
      std::get<1>(desc).AddInteger(
          "size-zstd-total-comp",
          static_cast<int64_t>(sub_sequence.GetRawSize()));
      std::get<1>(desc).AddInteger(
          "size-zstd-" + sub_seq_name + "-comp",
          static_cast<int64_t>(sub_sequence.GetRawSize()));
    }

    std::get<0>(desc).Set(snd, decompress(std::move(sub_sequence)));

    if (!std::get<0>(desc).Get(snd).IsEmpty()) {
      std::get<1>(desc).AddInteger(
          "size-zstd-total-raw",
          static_cast<int64_t>(std::get<0>(desc).Get(snd).GetRawSize()));
      std::get<1>(desc).AddInteger(
          "size-zstd-" + sub_seq_name + "-raw",
          static_cast<int64_t>(std::get<0>(desc).Get(snd).GetRawSize()));
    }
  }
  std::get<1>(desc).AddDouble("time-zstd", watch.Check());
  return desc;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

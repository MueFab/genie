/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/bsc/decoder.h"

#include <libbsc.h>

#include <string>
#include <tuple>
#include <utility>

#include "genie/util/runtime_exception.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

// -----------------------------------------------------------------------------
core::AccessUnit::Subsequence Decompress(core::AccessUnit::Subsequence&& data) {
  const auto id = data.GetId();

  uint8_t bytes = core::Range2Bytes(GetSubsequence(id).range);
  if (id.first == core::GenDesc::kReadName) {
    bytes = 1;
  }
  util::DataBlock in = data.Move();

  UTILS_DIE_IF(bsc_init(0) != LIBBSC_NO_ERROR, "bsc initialization failed");

  int original_size = 0;
  int compressed_size = 0;
  UTILS_DIE_IF(bsc_block_info(static_cast<const unsigned char*>(in.GetData()),
                              LIBBSC_HEADER_SIZE, &compressed_size,
                              &original_size, 0) != LIBBSC_NO_ERROR,
               "bsc block info failed");

  util::DataBlock out(original_size, bytes);

  UTILS_DIE_IF(bsc_decompress(static_cast<const unsigned char*>(in.GetData()),
                              in.GetRawSize(),
                              static_cast<unsigned char*>(out.GetData()),
                              original_size, 0) != LIBBSC_NO_ERROR,
               "bsc decompression failed");

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
    const auto [kFst, kSnd] = sub_sequence.GetId();

    auto sub_sequence_name = std::string();
    if (GetDescriptor(std::get<0>(desc).GetId()).token_type) {
      sub_sequence_name = GetDescriptor(std::get<0>(desc).GetId()).name;
    } else {
      sub_sequence_name =
          GetDescriptor(std::get<0>(desc).GetId()).name + "-" +
          GetDescriptor(std::get<0>(desc).GetId()).sub_seqs[kSnd].name;
    }
    if (!sub_sequence.IsEmpty()) {
      std::get<1>(desc).AddInteger(
          "Size-bsc-total-comp",
          static_cast<int64_t>(sub_sequence.GetRawSize()));
      std::get<1>(desc).AddInteger(
          "Size-bsc-" + sub_sequence_name + "-comp",
          static_cast<int64_t>(sub_sequence.GetRawSize()));
    }

    std::get<0>(desc).Set(kSnd, Decompress(std::move(sub_sequence)));

    if (!std::get<0>(desc).Get(kSnd).IsEmpty()) {
      std::get<1>(desc).AddInteger(
          "Size-bsc-total-raw",
          static_cast<int64_t>(std::get<0>(desc).Get(kSnd).GetRawSize()));
      std::get<1>(desc).AddInteger(
          "Size-bsc-" + sub_sequence_name + "-raw",
          static_cast<int64_t>(std::get<0>(desc).Get(kSnd).GetRawSize()));
    }
  }
  std::get<1>(desc).AddDouble("time-bsc", watch.Check());
  return desc;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

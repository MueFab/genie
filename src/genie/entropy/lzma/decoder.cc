/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/lzma/decoder.h"

#include <lzma.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "genie/util/runtime_exception.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::lzma {

// -----------------------------------------------------------------------------
core::AccessUnit::Subsequence decompress(core::AccessUnit::Subsequence&& data) {
  const auto id = data.GetId();

  uint8_t bytes = core::Range2Bytes(GetSubsequence(id).range);
  if (id.first == core::GenDesc::kReadName) {
    bytes = 1;
  }
  util::DataBlock in = data.Move();

  lzma_stream strm = LZMA_STREAM_INIT;
  UTILS_DIE_IF(
      lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK,
      "lzma initialization failed");

  strm.next_in = static_cast<const unsigned char*>(in.GetData());
  strm.avail_in = in.GetRawSize();
  size_t out_capacity =
      in.GetRawSize() * 2;  // Initial guess; typically this would be adjusted
                            // based on expected compression ratio
  util::DataBlock out(out_capacity, bytes);

  // Decompress in a loop, expanding the output buffer as needed
  while (true) {
    strm.next_out = static_cast<uint8_t*>(out.GetData()) + strm.total_out;
    strm.avail_out = out_capacity - strm.total_out;

    const auto ret = lzma_code(&strm, LZMA_FINISH);

    if (ret == LZMA_STREAM_END) {
      // Finished decompressing
      out.Resize(
          strm.total_out);  // Resize to the actual Size of decompressed data
      break;
    }

    UTILS_DIE_IF(ret != LZMA_OK,
                 "LZMA decompression failed: " + std::to_string(ret));

    if (strm.avail_out == 0) {
      // Output buffer is full, so expand it
      out_capacity *= 2;
      out.Resize(out_capacity);
    }
  }

  // Clean up
  lzma_end(&strm);

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
  for (auto& subseq : std::get<0>(desc)) {
    if (subseq.IsEmpty()) {
      continue;
    }
    const auto [fst, snd] = subseq.GetId();

    auto subseq_name = std::string();
    if (GetDescriptor(std::get<0>(desc).GetId()).token_type) {
      subseq_name = GetDescriptor(std::get<0>(desc).GetId()).name;
    } else {
      subseq_name = GetDescriptor(std::get<0>(desc).GetId()).name + "-" +
                    GetDescriptor(std::get<0>(desc).GetId()).sub_seqs[snd].name;
    }
    if (!subseq.IsEmpty()) {
      std::get<1>(desc).AddInteger("size-lzma-total-comp",
                                   static_cast<int64_t>(subseq.GetRawSize()));
      std::get<1>(desc).AddInteger("size-lzma-" + subseq_name + "-comp",
                                   static_cast<int64_t>(subseq.GetRawSize()));
    }

    std::get<0>(desc).Set(snd, decompress(std::move(subseq)));

    if (!std::get<0>(desc).Get(snd).IsEmpty()) {
      std::get<1>(desc).AddInteger(
          "size-lzma-total-raw",
          static_cast<int64_t>(std::get<0>(desc).Get(snd).GetRawSize()));
      std::get<1>(desc).AddInteger(
          "size-lzma-" + subseq_name + "-raw",
          static_cast<int64_t>(std::get<0>(desc).Get(snd).GetRawSize()));
    }
  }
  std::get<1>(desc).AddDouble("time-lzma", watch.Check());
  return desc;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

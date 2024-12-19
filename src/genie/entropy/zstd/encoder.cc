/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/zstd/encoder.h"

#include <zstd.h>

#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/entropy/zstd/param_decoder.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::zstd {

// -----------------------------------------------------------------------------
template <typename T>
void FillDecoder(const core::GenomicDescriptorProperties& desc,
                 T& decoder_config) {
  for (const auto& sub_sequence : desc.sub_seqs) {
    const auto bits_p2 = core::Range2Bytes(sub_sequence.range) * 8;
    auto sub_sequence_cfg = Subsequence(bits_p2);
    decoder_config.SetSubsequenceCfg(
        static_cast<uint8_t>(sub_sequence.id.second),
        std::move(sub_sequence_cfg));
  }
}

// -----------------------------------------------------------------------------
void StoreParameters(core::GenDesc desc,
                     core::parameter::DescriptorSubSequenceCfg& parameter_set) {
  auto descriptor_configuration =
      std::make_unique<core::parameter::desc_pres::DescriptorPresent>();

  auto decoder_config = std::make_unique<DecoderRegular>(desc);
  FillDecoder(GetDescriptor(desc), *decoder_config);
  descriptor_configuration->SetDecoder(std::move(decoder_config));

  parameter_set = core::parameter::DescriptorSubSequenceCfg();
  parameter_set.Set(std::move(descriptor_configuration));
}

// -----------------------------------------------------------------------------
core::AccessUnit::Subsequence compress(core::AccessUnit::Subsequence&& in) {
  const size_t num_symbols = in.GetNumSymbols();
  util::DataBlock input_buffer = in.Move();
  util::DataBlock output_buffer(ZSTD_compressBound(input_buffer.GetRawSize()),
                                1);
  const size_t compressed_size =
      ZSTD_compress(output_buffer.GetData(), output_buffer.GetRawSize(),
                    input_buffer.GetData(), input_buffer.GetRawSize(), 3);
  UTILS_DIE_IF(ZSTD_isError(compressed_size),
               "ZSTD compression failed: " +
                   std::string(ZSTD_getErrorName(compressed_size)));
  output_buffer.Resize(compressed_size);

  core::AccessUnit::Subsequence out(in.GetId());
  out.AnnotateNumSymbols(num_symbols);
  out.Set(std::move(output_buffer));
  return out;
}

// -----------------------------------------------------------------------------
core::EntropyEncoder::entropy_coded Encoder::Process(
    core::AccessUnit::Descriptor& desc) {
  entropy_coded ret;
  const util::Watch watch;
  std::get<1>(ret) = std::move(desc);
  for (auto& sub_sequence : std::get<1>(ret)) {
    if (!sub_sequence.IsEmpty()) {
      // add compressed payload
      const auto [fst, snd] = sub_sequence.GetId();

      std::get<2>(ret).AddInteger(
          "size-zstd-total-raw",
          static_cast<int64_t>(sub_sequence.GetRawSize()));
      auto sub_seq_name = std::string();
      if (GetDescriptor(std::get<1>(ret).GetId()).token_type) {
        sub_seq_name = GetDescriptor(std::get<1>(ret).GetId()).name;
      } else {
        sub_seq_name =
            GetDescriptor(std::get<1>(ret).GetId()).name + "-" +
            GetDescriptor(std::get<1>(ret).GetId()).sub_seqs[snd].name;
      }
      std::get<2>(ret).AddInteger(
          "size-zstd-" + sub_seq_name + "-raw",
          static_cast<int64_t>(sub_sequence.GetRawSize()));

      std::get<1>(ret).Set(snd, compress(std::move(sub_sequence)));

      if (!std::get<1>(ret).Get(snd).IsEmpty()) {
        std::get<2>(ret).AddInteger(
            "size-zstd-total-comp",
            static_cast<int64_t>(std::get<1>(ret).Get(snd).GetRawSize()));
        std::get<2>(ret).AddInteger(
            "size-zstd-" + sub_seq_name + "-comp",
            static_cast<int64_t>(std::get<1>(ret).Get(snd).GetRawSize()));
      }
    } else {
      // add empty payload
      std::get<1>(ret).Set(sub_sequence.GetId().second,
                           core::AccessUnit::Subsequence(
                               sub_sequence.GetId(), util::DataBlock(0, 1)));
    }
  }
  StoreParameters(std::get<1>(ret).GetId(), std::get<0>(ret));
  std::get<2>(ret).AddDouble("time-zstd", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------
Encoder::Encoder(const bool write_out_streams)
    : write_out_streams_(write_out_streams) {}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

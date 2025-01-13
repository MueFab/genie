/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/lzma/encoder.h"

#include <lzma.h>

#include <atomic>
#include <memory>
#include <string>
#include <utility>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/entropy/lzma/param_decoder.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::lzma {

// -----------------------------------------------------------------------------

template <typename T>
void FillDecoder(const core::GenomicDescriptorProperties& desc,
                 T& decoder_config) {
  for (const auto& sub_seq : desc.sub_seqs) {
    const auto bits_p2 = core::Range2Bytes(sub_seq.range) * 8;
    auto sub_seq_cfg = Subsequence(bits_p2);
    decoder_config.SetSubsequenceCfg(static_cast<uint8_t>(sub_seq.id.second),
                                     std::move(sub_seq_cfg));
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

core::AccessUnit::Subsequence Compress(core::AccessUnit::Subsequence&& in) {
  const size_t num_symbols = in.GetNumSymbols();
  util::DataBlock input_buffer = in.Move();

  lzma_stream strm = LZMA_STREAM_INIT;  // Initialize the lzma_stream structure

  UTILS_DIE_IF(lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT,
                                 LZMA_CHECK_CRC64) != LZMA_OK,
               "lzma initialization failed");

  strm.next_in = static_cast<const unsigned char*>(input_buffer.GetData());
  strm.avail_in = input_buffer.GetRawSize();

  const size_t out_size = lzma_stream_buffer_bound(input_buffer.GetRawSize());
  util::DataBlock output_buffer(out_size, 1);

  strm.next_out = static_cast<unsigned char*>(output_buffer.GetData());
  strm.avail_out = output_buffer.GetRawSize();
  lzma_ret ret = LZMA_OK;
  while (ret == LZMA_OK) {
    ret = lzma_code(&strm, LZMA_FINISH);
    UTILS_DIE_IF(ret != LZMA_STREAM_END && ret != LZMA_OK,
                 "lzma compression failed: " + std::to_string(ret));
  }

  lzma_end(&strm);

  output_buffer.Resize(strm.total_out);

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
  for (auto& sub_seq : std::get<1>(ret)) {
    if (!sub_seq.IsEmpty()) {
      // add compressed payload
      const auto [kFst, kSnd] = sub_seq.GetId();

      std::get<2>(ret).AddInteger("size-lzma-total-raw",
                                  static_cast<int64_t>(sub_seq.GetRawSize()));
      auto sub_seq_name = std::string();
      if (GetDescriptor(std::get<1>(ret).GetId()).token_type) {
        sub_seq_name = GetDescriptor(std::get<1>(ret).GetId()).name;
      } else {
        sub_seq_name =
            GetDescriptor(std::get<1>(ret).GetId()).name + "-" +
            GetDescriptor(std::get<1>(ret).GetId()).sub_seqs[kSnd].name;
      }
      std::get<2>(ret).AddInteger("size-lzma-" + sub_seq_name + "-raw",
                                  static_cast<int64_t>(sub_seq.GetRawSize()));

      std::get<1>(ret).Set(kSnd, Compress(std::move(sub_seq)));

      if (!std::get<1>(ret).Get(kSnd).IsEmpty()) {
        std::get<2>(ret).AddInteger(
            "size-lzma-total-comp",
            static_cast<int64_t>(std::get<1>(ret).Get(kSnd).GetRawSize()));
        std::get<2>(ret).AddInteger(
            "size-lzma-" + sub_seq_name + "-comp",
            static_cast<int64_t>(std::get<1>(ret).Get(kSnd).GetRawSize()));
      }
    } else {
      // add empty payload
      std::get<1>(ret).Set(sub_seq.GetId().second,
                           core::AccessUnit::Subsequence(
                               sub_seq.GetId(), util::DataBlock(0, 1)));
    }
  }
  StoreParameters(std::get<1>(ret).GetId(), std::get<0>(ret));
  std::get<2>(ret).AddDouble("time-lzma", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------

Encoder::Encoder(const bool write_out_streams)
    : write_out_streams_(write_out_streams) {}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

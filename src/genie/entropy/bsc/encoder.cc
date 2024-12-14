/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/bsc/encoder.h"

#include <libbsc.h>

#include <atomic>
#include <memory>
#include <string>
#include <utility>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/entropy/bsc/param_decoder.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

// -----------------------------------------------------------------------------
template <typename T>
void FillDecoder(const core::GenomicDescriptorProperties& desc,
                 T& decoder_config) {
  for (const auto& sub_sequence_descriptor : desc.sub_seqs) {
    const auto bits_p2 = core::Range2Bytes(sub_sequence_descriptor.range) * 8;
    auto sub_sequence_cfg = Subsequence(bits_p2);
    decoder_config.SetSubsequenceCfg(
        static_cast<uint8_t>(sub_sequence_descriptor.id.second),
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
core::AccessUnit::Subsequence Compress(core::AccessUnit::Subsequence&& in) {
  const size_t num_symbols = in.GetNumSymbols();
  util::DataBlock input_buffer = in.Move();
  util::DataBlock output_buffer(input_buffer.GetRawSize() + LIBBSC_HEADER_SIZE,
                                1);

  UTILS_DIE_IF(bsc_init(0) != LIBBSC_NO_ERROR, "bsc initialization failed");
  const int compressed_size =
      bsc_compress(static_cast<const unsigned char*>(input_buffer.GetData()),
                   static_cast<unsigned char*>(output_buffer.GetData()),
                   static_cast<int>(input_buffer.GetRawSize()),
                   LIBBSC_DEFAULT_LZPHASHSIZE, LIBBSC_DEFAULT_LZPMINLEN,
                   LIBBSC_DEFAULT_BLOCKSORTER, LIBBSC_DEFAULT_CODER, 0);
  UTILS_DIE_IF(compressed_size < 0,
               "bsc compression failed: " + std::to_string(compressed_size));
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
  for (auto& sub_descriptor : std::get<1>(ret)) {
    if (!sub_descriptor.IsEmpty()) {
      // add compressed payload
      const auto [kFst, kSnd] = sub_descriptor.GetId();

      std::get<2>(ret).AddInteger(
          "size-bsc-total-raw",
          static_cast<int64_t>(sub_descriptor.GetRawSize()));
      auto sub_seq_name = std::string();
      if (GetDescriptor(std::get<1>(ret).GetId()).token_type) {
        sub_seq_name = GetDescriptor(std::get<1>(ret).GetId()).name;
      } else {
        sub_seq_name =
            GetDescriptor(std::get<1>(ret).GetId()).name + "-" +
            GetDescriptor(std::get<1>(ret).GetId()).sub_seqs[kSnd].name;
      }
      std::get<2>(ret).AddInteger(
          "size-bsc-" + sub_seq_name + "-raw",
          static_cast<int64_t>(sub_descriptor.GetRawSize()));

      std::get<1>(ret).Set(kSnd, Compress(std::move(sub_descriptor)));

      if (!std::get<1>(ret).Get(kSnd).IsEmpty()) {
        std::get<2>(ret).AddInteger(
            "size-bsc-total-comp",
            static_cast<int64_t>(std::get<1>(ret).Get(kSnd).GetRawSize()));
        std::get<2>(ret).AddInteger(
            "size-bsc-" + sub_seq_name + "-comp",
            static_cast<int64_t>(std::get<1>(ret).Get(kSnd).GetRawSize()));
      }
    } else {
      // add empty payload
      std::get<1>(ret).Set(sub_descriptor.GetId().second,
                           core::AccessUnit::Subsequence(
                               sub_descriptor.GetId(), util::DataBlock(0, 1)));
    }
  }
  StoreParameters(std::get<1>(ret).GetId(), std::get<0>(ret));
  std::get<2>(ret).AddDouble("time-bsc", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------
Encoder::Encoder(const bool write_out_streams)
    : writeOutStreams(write_out_streams) {}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------

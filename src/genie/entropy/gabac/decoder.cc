/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/decoder.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/entropy/gabac/decode_desc_sub_seq.h"
#include "genie/entropy/gabac/decode_transformed_sub_seq.h"
#include "genie/entropy/gabac/mismatch_decoder.h"
#include "genie/entropy/gabac/stream_handler.h"
#include "genie/util/runtime_exception.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
core::AccessUnit::Descriptor DecompressTokens(
    const EncodingConfiguration& conf0, const EncodingConfiguration&,
    core::AccessUnit::Subsequence&& data) {
  core::AccessUnit::Subsequence in = std::move(data);
  util::DataBlock remaining_data = std::move(in.Move());
  core::AccessUnit::Descriptor ret(in.GetId().first);
  if (remaining_data.Empty()) {
    return ret;
  }
  size_t offset = 6;
  UTILS_DIE_IF(offset >= remaining_data.GetRawSize(),
               "Token type stream smaller than expected");
  uint16_t num_token_type_descriptors = 0;
  {
    constexpr size_t read_ahead = 6;
    auto tmp = util::DataBlock(static_cast<uint8_t*>(remaining_data.GetData()),
                               read_ahead, remaining_data.GetWordSize());
    IBufferStream stream(&tmp);
    util::BitReader reader(stream);

    reader.Read<uint32_t>();
    num_token_type_descriptors = reader.Read<uint16_t>();
  }
  int32_t type_num = -1;
  for (size_t i = 0; i < num_token_type_descriptors; ++i) {
    uint64_t num_symbols = 0;
    size_t mapped_type_id = 0;
    {
      uint16_t method = 0;
      uint16_t type_id = 0;
      const size_t read_ahead =
          std::min<size_t>(11, remaining_data.GetRawSize() - offset - 1);
      UTILS_DIE_IF(offset + read_ahead >= remaining_data.GetRawSize(),
                   "Token type stream smaller than expected");
      auto tmp = util::DataBlock(
          static_cast<uint8_t*>(remaining_data.GetData()) + offset, read_ahead,
          remaining_data.GetWordSize());
      IBufferStream stream(&tmp);
      util::BitReader reader(stream);

      type_id = reader.Read<uint16_t>(4);
      method = reader.Read<uint16_t>(4);
      if (type_id == 0) type_num++;
      mapped_type_id = (type_num << 4u) | (type_id & 0xfu);  // NOLINT

      UTILS_DIE_IF(method != 3, "Only CABAC0 supported");
      offset++;
      offset += StreamHandler::ReadU7(stream, num_symbols);
    }

    std::vector<util::DataBlock> transformed_seqs;
    for (size_t j = 0;
         j < conf0.GetSubSeqConfig().GetNumTransformSubSeqConfigs(); ++j) {
      size_t payload_size = 0;
      if (j < conf0.GetSubSeqConfig().GetNumTransformSubSeqConfigs() - 1) {
        auto tmp = util::DataBlock(
            static_cast<uint8_t*>(remaining_data.GetData()) + offset, 4,
            remaining_data.GetWordSize());
        IBufferStream stream(&tmp);
        util::BitReader reader(stream);
        payload_size = reader.Read<uint32_t>();
        offset += 4;
      } else {
        payload_size = remaining_data.GetRawSize() - offset;
      }
      auto num_transformed_symbols = static_cast<uint32_t>(num_symbols);
      if (payload_size > 0) {
        if (conf0.GetSubSeqConfig().GetNumTransformSubSeqConfigs() > 1) {
          auto tmp = util::DataBlock(
              static_cast<uint8_t*>(remaining_data.GetData()) + offset, 4,
              remaining_data.GetWordSize());
          IBufferStream stream(&tmp);
          util::BitReader reader(stream);
          num_transformed_symbols = reader.Read<uint32_t>();
          offset += 4;
          payload_size -= 4;
        }
      }
      auto tmp = util::DataBlock(
          static_cast<uint8_t*>(remaining_data.GetData()) + offset,
          payload_size, remaining_data.GetWordSize());
      offset +=
          DecodeTransformSubSeq(conf0.GetSubSeqConfig().GetTransformSubSeqCfg(
                                    static_cast<uint8_t>(j)),
                                num_transformed_symbols, &tmp, 4);

      transformed_seqs.emplace_back(std::move(tmp));
    }

    DoInverseSubsequenceTransform(conf0.GetSubSeqConfig(), &transformed_seqs);

    while (ret.GetSize() < mapped_type_id) {
      ret.Add(core::AccessUnit::Subsequence(
          1, core::GenSubIndex{core::GenDesc::kReadName,
                               static_cast<uint16_t>(ret.GetSize())}));
    }
    ret.Add(core::AccessUnit::Subsequence(
        std::move(transformed_seqs.front()),
        core::GenSubIndex{core::GenDesc::kReadName,
                          static_cast<uint16_t>(mapped_type_id)}));
  }
  return ret;
}

// -----------------------------------------------------------------------------
core::AccessUnit::Subsequence Decoder::Decompress(
    const EncodingConfiguration& conf, core::AccessUnit::Subsequence&& data,
    bool mm_coder_enabled) {
  core::AccessUnit::Subsequence in = std::move(data);
  auto id = in.GetId();

  if (GetDescriptor(in.GetId().first)
          .GetSubSeq(static_cast<uint8_t>(in.GetId().second))
          .mismatch_decoding &&
      mm_coder_enabled) {
    in.AttachMismatchDecoder(
        std::make_unique<MismatchDecoder>(in.Move(), conf));
    return in;
  }

  // Interface to GABAC library
  util::DataBlock buffer = in.Move();
  IBufferStream in_stream(&buffer, 0);

  uint8_t bytes = core::Range2Bytes(GetSubsequence(id).range);
  util::DataBlock tmp(0, bytes);
  OBufferStream output_buffer(&tmp);

  // Setup
  constexpr size_t gabac_block_size =
      0;  // 0 means single block (block size is equal to input Size)
  std::ostream* const gabac_log_output_stream = &std::cerr;
  const IoConfiguration gabac_io_setup = {&in_stream,
                                          1,
                                          nullptr,
                                          &output_buffer,
                                          bytes,
                                          gabac_block_size,
                                          gabac_log_output_stream,
                                          IoConfiguration::LogLevel::LOG_TRACE};
  constexpr bool gabac_decoding_mode = true;

  // Run
  Run(gabac_io_setup, conf, gabac_decoding_mode);

  output_buffer.Flush(&tmp);
  return {std::move(tmp), in.GetId()};
}

// -----------------------------------------------------------------------------
std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats>
Decoder::Process(const core::parameter::DescriptorSubSequenceCfg& param,
                 core::AccessUnit::Descriptor& d, bool mm_coder_enabled) {
  util::Watch watch;
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> desc;
  std::get<0>(desc) = std::move(d);
  const auto& param_desc =
      dynamic_cast<const core::parameter::desc_pres::DescriptorPresent&>(
          param.Get());
  if (GetDescriptor(std::get<0>(desc).GetId()).token_type) {
    size_t size = 0;
    for (const auto& s : std::get<0>(desc)) {
      size += s.GetNumSymbols() * sizeof(uint32_t);
    }

    if (size) {
      std::get<1>(desc).AddInteger("size-gabac-total-comp",
                                   static_cast<int64_t>(size));
      std::get<1>(desc).AddInteger(
          "size-gabac-" + GetDescriptor(std::get<0>(desc).GetId()).name +
              "-comp",
          static_cast<int64_t>(size));
    }

    const auto& token_param = dynamic_cast<const paramcabac::DecoderTokenType&>(
        param_desc.GetDecoder());
    auto conf0 = token_param.GetSubsequenceCfg(0);
    auto conf1 = token_param.GetSubsequenceCfg(1);
    std::get<0>(desc) =
        DecompressTokens(EncodingConfiguration(std::move(conf0)),
                         EncodingConfiguration(std::move(conf1)),
                         std::move(*std::get<0>(desc).begin()));

    if (size) {
      std::get<1>(desc).AddInteger(
          "size-gabac-total-raw",
          static_cast<int64_t>(std::get<0>(desc).begin()->GetRawSize()));
      std::get<1>(desc).AddInteger(
          "size-gabac-" + GetDescriptor(std::get<0>(desc).GetId()).name +
              "-raw",
          static_cast<int64_t>(std::get<0>(desc).begin()->GetRawSize()));
    }
  } else {
    for (auto& sub_sequence : std::get<0>(desc)) {
      if (sub_sequence.IsEmpty()) {
        continue;
      }
      auto [fst, snd] = sub_sequence.GetId();

      const auto& token_param = dynamic_cast<const paramcabac::DecoderRegular&>(
          param_desc.GetDecoder());
      auto conf0 = token_param.GetSubsequenceCfg(static_cast<uint8_t>(snd));

      if (!sub_sequence.IsEmpty()) {
        std::get<1>(desc).AddInteger(
            "size-gabac-total-comp",
            static_cast<int64_t>(sub_sequence.GetRawSize()));
        std::get<1>(desc).AddInteger(
            "size-gabac-" + GetDescriptor(std::get<0>(desc).GetId()).name +
                "-" +
                GetDescriptor(std::get<0>(desc).GetId()).sub_seqs[snd].name +
                "-comp",
            static_cast<int64_t>(sub_sequence.GetRawSize()));
      }

      std::get<0>(desc).Set(
          snd, Decompress(EncodingConfiguration(std::move(conf0)),
                          std::move(sub_sequence), mm_coder_enabled));

      if (!std::get<0>(desc).Get(snd).IsEmpty()) {
        std::get<1>(desc).AddInteger(
            "size-gabac-total-raw",
            static_cast<int64_t>(std::get<0>(desc).Get(snd).GetRawSize()));
        std::get<1>(desc).AddInteger(
            "size-gabac-" + GetDescriptor(std::get<0>(desc).GetId()).name +
                "-" +
                GetDescriptor(std::get<0>(desc).GetId()).sub_seqs[snd].name +
                "-raw",
            static_cast<int64_t>(std::get<0>(desc).Get(snd).GetRawSize()));
      }
    }
  }
  std::get<1>(desc).AddDouble("time-gabac", watch.Check());
  return desc;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

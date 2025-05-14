/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/encoder.h"

#include <iostream>
#include <string>
#include <utility>

#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

core::AccessUnit::Descriptor Encoder::CompressTokens(
    const EncodingConfiguration& conf0, core::AccessUnit::Descriptor&& in) {
  auto desc = std::move(in);
  util::DataBlock block(0, 1);
  OBufferStream stream(&block);
  util::BitWriter writer(stream);
  size_t num_streams = 0;
  for (const auto& subsequence : desc) {
    if (subsequence.GetNumSymbols()) {
      num_streams++;
    }
  }

  if (num_streams == 0) {
    auto ret_empty = core::AccessUnit::Descriptor(desc.GetId());
    // add empty payload
    ret_empty.Add(core::AccessUnit::Subsequence(
        {desc.GetId(), static_cast<uint16_t>(0)}, util::DataBlock(0, 1)));
    return ret_empty;
  }

  writer.WriteBits(desc.begin()->GetNumSymbols(), 32);
  writer.WriteBits(num_streams, 16);

  for (auto& subsequence : desc) {
    if (subsequence.GetNumSymbols()) {
      writer.WriteBits(subsequence.GetId().second & 0xfu, 4);
      writer.WriteBits(3, 4);
      Compress(conf0, std::move(subsequence)).Write(writer);
    }
  }

  stream.Flush(&block);
  core::AccessUnit::Descriptor ret(desc.GetId());
  ret.Add(core::AccessUnit::Subsequence({desc.GetId(), static_cast<uint8_t>(0)},
                                        std::move(block)));
  return ret;
}

// -----------------------------------------------------------------------------

core::AccessUnit::Subsequence Encoder::Compress(
    const EncodingConfiguration& conf, core::AccessUnit::Subsequence&& in) {
  // Interface to GABAC library
  core::AccessUnit::Subsequence data = std::move(in);
  size_t num_symbols = data.GetNumSymbols();
  util::DataBlock buffer = data.Move();
  uint8_t inputwordsize = buffer.GetWordSize();
  IBufferStream buffer_input_stream(&buffer);

  IBufferStream* buffer_dependency_stream = nullptr;
  if (data.GetDependency() != nullptr) {
    buffer_dependency_stream = new IBufferStream(data.GetDependency());
  }

  util::DataBlock outblock(0, 1);
  OBufferStream buffer_output_stream(&outblock);

  // Setup
  constexpr size_t gabac_block_size =
      0;  // 0 means single block (block size is equal to input Size)
  std::ostream* const gabac_log_output_stream = &std::cerr;
  const IoConfiguration gabac_io_setup = {&buffer_input_stream,
                                          inputwordsize,
                                          buffer_dependency_stream,
                                          &buffer_output_stream,
                                          1,
                                          gabac_block_size,
                                          gabac_log_output_stream,
                                          IoConfiguration::LogLevel::LOG_TRACE};
  constexpr bool gabac_decoding_mode = false;

  // Run
  Run(gabac_io_setup, conf, gabac_decoding_mode);

  buffer_output_stream.Flush(&outblock);
  core::AccessUnit::Subsequence out(data.GetId());
  out.AnnotateNumSymbols(num_symbols);
  out.Set(std::move(outblock));

  delete buffer_dependency_stream;

  return out;
}

// -----------------------------------------------------------------------------

core::EntropyEncoder::entropy_coded Encoder::Process(
    core::AccessUnit::Descriptor& desc) {

  // ----------------- Broken Standard Workaround ----------------------

  if (desc.GetId() == core::GenDesc::kRtype && !desc.IsEmpty() &&
      !desc.Get(0).IsEmpty() && desc.Get(0).tmp_rtype_issue_flag_) {
    for (size_t i = 0; i < desc.Get(0).GetNumSymbols(); ++i) {
      UTILS_DIE_IF(desc.Get(0).GetData().Get(i) == 6,
                   "Local Assembly + class HM reads + gabac is broken in the "
                   "ISO standard.");
    }
  }

  // -------------------------------------------------------------------

  entropy_coded ret;
  const util::Watch watch;
  std::get<1>(ret) = std::move(desc);
  if (!GetDescriptor(std::get<1>(ret).GetId()).token_type) {
    for (auto& subdesc : std::get<1>(ret)) {
      if (!subdesc.IsEmpty()) {
        const auto& conf = config_set_.GetConfAsGabac(subdesc.GetId());
        // add compressed payload
        const auto [kFst, kSnd] = subdesc.GetId();

        std::get<2>(ret).AddInteger("size-gabac-total-raw",
                                    static_cast<int64_t>(subdesc.GetRawSize()));
        std::get<2>(ret).AddInteger(
            "size-gabac-" + GetDescriptor(std::get<1>(ret).GetId()).name + "-" +
                GetDescriptor(std::get<1>(ret).GetId()).sub_seqs[kSnd].name +
                "-raw",
            static_cast<int64_t>(subdesc.GetRawSize()));

        std::get<1>(ret).Set(kSnd, Compress(conf, std::move(subdesc)));

        if (!std::get<1>(ret).Get(kSnd).IsEmpty()) {
          std::get<2>(ret).AddInteger(
              "size-gabac-total-comp",
              static_cast<int64_t>(std::get<1>(ret).Get(kSnd).GetRawSize()));
          std::get<2>(ret).AddInteger(
              "size-gabac-" + GetDescriptor(std::get<1>(ret).GetId()).name +
                  "-" +
                  GetDescriptor(std::get<1>(ret).GetId()).sub_seqs[kSnd].name +
                  "-comp",
              static_cast<int64_t>(std::get<1>(ret).Get(kSnd).GetRawSize()));
        }
      } else {
        // add empty payload
        std::get<1>(ret).Set(subdesc.GetId().second,
                             core::AccessUnit::Subsequence(
                                 subdesc.GetId(), util::DataBlock(0, 1)));
      }
    }
    config_set_.StoreParameters(std::get<1>(ret).GetId(), std::get<0>(ret));
  } else {
    size_t size = 0;
    for (const auto& s : std::get<1>(ret)) {
      size += s.GetNumSymbols() * sizeof(uint32_t);
    }
    const auto& conf = config_set_.GetConfAsGabac(
        {std::get<1>(ret).GetId(), static_cast<uint16_t>(0)});
    std::get<1>(ret) = CompressTokens(conf, std::move(std::get<1>(ret)));
    config_set_.StoreParameters(std::get<1>(ret).GetId(), std::get<0>(ret));

    if (size) {
      std::get<2>(ret).AddInteger("size-gabac-total-raw",
                                  static_cast<int64_t>(size));
      std::get<2>(ret).AddInteger(
          "size-gabac-" + GetDescriptor(std::get<1>(ret).GetId()).name + "-raw",
          static_cast<int64_t>(size));
      std::get<2>(ret).AddInteger(
          "size-gabac-total-comp",
          static_cast<int64_t>(std::get<1>(ret).begin()->GetRawSize()));
      std::get<2>(ret).AddInteger(
          "size-gabac-" + GetDescriptor(std::get<1>(ret).GetId()).name +
              "-comp",
          static_cast<int64_t>(std::get<1>(ret).begin()->GetRawSize()));
    }
  }
  std::get<2>(ret).AddDouble("time-gabac", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------

Encoder::Encoder(const bool write_out_streams)
    : write_out_streams_(write_out_streams) {}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

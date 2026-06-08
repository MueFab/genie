/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#include "genie/entropy/lzma/encoder.h"

#include <lzma.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include "genie/entropy/lzma/param_decoder.h"
#include "genie/util/stop_watch.h"
// -----------------------------------------------------------------------------

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

LZMAEncoder::LZMAEncoder()
    : level(LZMA_DEFAULT_LEVEL),
      dictSize(LZMA_DEFAULT_DIC_SIZE),
      lc(LZMA_DEFAULT_LC),
      lp(LZMA_DEFAULT_LP),
      pb(LZMA_DEFAULT_PB),
      fb(LZMA_DEFAULT_FB),
      numThreads(LZMA_DEFAULT_THREADS) {}

void LZMAEncoder::encode(std::stringstream &input, std::stringstream &output) {
    const std::string str = input.str();
    const size_t srcLen = str.size();
    if (srcLen == 0) return;

    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_easy_encoder(&strm, level, LZMA_CHECK_CRC64);
    if (ret != LZMA_OK) {
        std::cerr << "lzma initialization failed\n";
        return;
    }

    strm.next_in = (const uint8_t*)str.c_str();
    strm.avail_in = srcLen;

    const size_t out_bound = lzma_stream_buffer_bound(srcLen);
    std::vector<uint8_t> output_buffer(out_bound);

    strm.next_out = output_buffer.data();
    strm.avail_out = output_buffer.size();

    ret = lzma_code(&strm, LZMA_FINISH);
    if (ret != LZMA_STREAM_END && ret != LZMA_OK) {
        std::cerr << "lzma compression failed: " << ret << "\n";
        lzma_end(&strm);
        return;
    }

    size_t compressed_size = output_buffer.size() - strm.avail_out;
    output.write((const char *)output_buffer.data(), compressed_size);
    lzma_end(&strm);
}

void LZMAEncoder::decode(std::stringstream &input, std::stringstream &output) {
    const std::string str = input.str();
    const size_t srcLen = str.size();
    if (srcLen == 0) return;

    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_stream_decoder(&strm, UINT64_MAX, 0);
    if (ret != LZMA_OK) {
        std::cerr << "lzma decoder initialization failed\n";
        return;
    }

    strm.next_in = (const uint8_t*)str.c_str();
    strm.avail_in = srcLen;

    std::vector<uint8_t> temp_out(65536);
    strm.next_out = temp_out.data();
    strm.avail_out = temp_out.size();

    while (true) {
        ret = lzma_code(&strm, LZMA_RUN);
        if (ret == LZMA_STREAM_END) {
            size_t written = temp_out.size() - strm.avail_out;
            if (written > 0) {
                output.write((const char*)temp_out.data(), written);
            }
            break;
        }
        if (ret != LZMA_OK) {
            std::cerr << "lzma decompression failed: " << ret << "\n";
            break;
        }
        if (strm.avail_out == 0) {
            output.write((const char*)temp_out.data(), temp_out.size());
            strm.next_out = temp_out.data();
            strm.avail_out = temp_out.size();
        }
    }
    lzma_end(&strm);
}

genie::core::parameter::annotation::AlgorithmParameters LZMAParameters::convertToAlgorithmParameters()
    const {
    uint8_t n_pars = 7;
    std::vector<uint8_t> par_ID{1, 2, 3, 4, 5, 6, 7};
    std::vector<genie::core::DataType> par_type{genie::core::DataType::UINT8, genie::core::DataType::UINT32,
                                                genie::core::DataType::UINT8, genie::core::DataType::UINT8,
                                                genie::core::DataType::UINT8, genie::core::DataType::UINT16,
                                                genie::core::DataType::UINT8};
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<uint32_t> values{static_cast<uint32_t>(level), static_cast<uint32_t>(dictSize), static_cast<uint32_t>(lc),
                                 static_cast<uint32_t>(lp),    static_cast<uint32_t>(pb),       static_cast<uint32_t>(fb),
                                 static_cast<uint32_t>(numThreads)};

    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto idx_i = 0; idx_i < n_pars; ++idx_i) {
        if (par_type.at(idx_i) == core::DataType::UINT8) {
            par_val.push_back(core::parameter::annotation::parameterToVector<uint8_t>(
                {static_cast<uint8_t>(values.at(idx_i))}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
        if (par_type.at(idx_i) == core::DataType::UINT16) {
            par_val.push_back(core::parameter::annotation::parameterToVector<uint16_t>(
                {static_cast<uint16_t>(values.at(idx_i))}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
        if (par_type.at(idx_i) == core::DataType::UINT32) {
            par_val.push_back(core::parameter::annotation::parameterToVector<uint32_t>(
                {static_cast<uint32_t>(values.at(idx_i))}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
    }

    return genie::core::parameter::annotation::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

genie::core::parameter::annotation::CompressorParameterSet LZMAParameters::compressorParameterSet(
    uint8_t compressor_ID) const {
    std::vector<genie::core::AlgoID> LZMAalgorithm_ID{genie::core::AlgoID::LZMA};
    uint8_t n_compressor_steps = 1;
    std::vector<uint8_t> compressor_step_ID{0};
    std::vector<bool> use_default_pars{true};
    std::vector<genie::core::parameter::annotation::AlgorithmParameters> algorithm_parameters;
    std::vector<uint8_t> n_in_vars{0};
    std::vector<std::vector<uint8_t>> in_var_ID{{0}};
    std::vector<std::vector<uint8_t>> prev_step_ID;
    std::vector<std::vector<uint8_t>> prev_out_var_ID;
    std::vector<uint8_t> n_completed_out_vars{0};
    std::vector<std::vector<uint8_t>> completed_out_var_ID;

    return genie::core::parameter::annotation::CompressorParameterSet(
        compressor_ID, n_compressor_steps, compressor_step_ID, LZMAalgorithm_ID, use_default_pars, algorithm_parameters,
        n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID);
}

}  // namespace genie::entropy::lzma

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

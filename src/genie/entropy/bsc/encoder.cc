/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/bsc/encoder.h"

#include <libbsc.h>

#include <atomic>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
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

BSCEncoder::BSCEncoder()
    : lzpHashSize(LIBBSC_DEFAULT_LZPHASHSIZE),
      lzpMinLen(LIBBSC_DEFAULT_LZPMINLEN),
      blockSorter(LIBBSC_DEFAULT_BLOCKSORTER),
      coder(LIBBSC_DEFAULT_CODER) {}

void BSCEncoder::encode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    if (srcLen == 0) return;

    unsigned long int dstLenTot = (srcLen + 20) / 20 * 21 + (1U << 16U) + LIBBSC_HEADER_SIZE;
    std::vector<unsigned char> tmpBuffer(dstLenTot);

    bsc_init(LIBBSC_FEATURE_FASTMODE);

    int len = bsc_compress((const unsigned char *)input.str().c_str(), tmpBuffer.data(), srcLen,
                           lzpHashSize, lzpMinLen, blockSorter, coder, LIBBSC_FEATURE_FASTMODE);
    if (len <= 0) {
        std::cerr << "error with bsc compression\n";
        return;
    }
    output.write((const char *)tmpBuffer.data(), len);
}

void BSCEncoder::decode(std::stringstream &input, std::stringstream &output) {
    const std::string str = input.str();
    const size_t srcLen = str.size();
    if (srcLen == 0) return;

    bsc_init(LIBBSC_FEATURE_FASTMODE);

    int blockSize = 0, dataSize = 0;
    int ret = bsc_block_info((const unsigned char *)str.c_str(), srcLen, &blockSize, &dataSize, LIBBSC_FEATURE_FASTMODE);
    if (ret != LIBBSC_NO_ERROR) {
        std::cerr << "error with bsc block info\n";
        return;
    }

    std::vector<unsigned char> tmpBuffer(dataSize);
    ret = bsc_decompress((const unsigned char *)str.c_str(), srcLen, tmpBuffer.data(), dataSize, LIBBSC_FEATURE_FASTMODE);
    if (ret != LIBBSC_NO_ERROR) {
        std::cerr << "error with decompression\n";
        return;
    }
    output.write((const char *)tmpBuffer.data(), dataSize);
}

genie::core::parameter::annotation::AlgorithmParameters BSCParameters::convertToAlgorithmParameters() const {
    uint8_t n_pars = 4;
    std::vector<uint8_t> par_ID{1, 2, 3, 4};
    const std::vector<genie::core::DataType> par_type{genie::core::DataType::UINT8, genie::core::DataType::UINT8,
                                                      genie::core::DataType::UINT8, genie::core::DataType::UINT8};
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<uint8_t> values{static_cast<uint8_t>(lzpHashSize), static_cast<uint8_t>(lzpMinLen),
                                static_cast<uint8_t>(blockSorter), static_cast<uint8_t>(coder)};

    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));

    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto idx_i = 0; idx_i < n_pars; ++idx_i) {
        if (par_type.at(idx_i) == core::DataType::UINT8) {
            par_val.push_back(core::parameter::annotation::parameterToVector<uint8_t>(
                {values.at(idx_i)}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
    }

    return genie::core::parameter::annotation::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

genie::core::parameter::annotation::CompressorParameterSet BSCParameters::compressorParameterSet(
    uint8_t compressor_ID) const {
    std::vector<genie::core::AlgoID> BSCalgorithm_ID{genie::core::AlgoID::BSC};
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
        compressor_ID, n_compressor_steps, compressor_step_ID, BSCalgorithm_ID, use_default_pars, algorithm_parameters,
        n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID);
}

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

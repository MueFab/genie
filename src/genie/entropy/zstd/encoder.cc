/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 * @brief Implementation of Zstd-based entropy encoding for Genie.
 *
 * Provides functionality for compressing subsequences,
 * managing descriptor parameters,
 * and tracking performance metrics using the Zstd library.
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/zstd/encoder.h"

#include <zstd.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include "genie/entropy/zstd/param_decoder.h"
#include "genie/util/stop_watch.h"
#include "codecs/include/mpegg-codecs.h"

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

ZSTDEncoder::ZSTDEncoder() : use_dictionary_flag(false), dictionary_size(0), dictionary{} {}

void ZSTDEncoder::encode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *destination = nullptr;
    size_t destLen = srcLen;

    int ret = mpegg_zstd_compress(&destination, &destLen, (const unsigned char *)input.str().c_str(), srcLen, 0);

    if (ret != 0) {
        std::cerr << "error with zstd compression\n";
    }
    output.write((const char *)destination, destLen);
    if (destination) free(destination);
}

void ZSTDEncoder::decode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *destination = nullptr;
    size_t destLen = srcLen;
    int ret = mpegg_zstd_decompress(&destination, &destLen, (const unsigned char *)input.str().c_str(), srcLen);
    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    output.write((const char *)destination, destLen);
    if (destination) free(destination);
}

genie::core::parameter::annotation::AlgorithmParameters ZSTDParameters::convertToAlgorithmParameters()
    const {
    uint8_t n_pars = 3;
    std::vector<uint8_t> par_ID{1, 2, 3};
    std::vector<genie::core::DataType> par_type{genie::core::DataType::BOOL, genie::core::DataType::UINT16,
                                                genie::core::DataType::STRING};
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<uint16_t> values{use_dictionary_flag, 0, 0};

    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto idx_i = 0; idx_i < n_pars; ++idx_i) {
        if (par_type.at(idx_i) == core::DataType::BOOL) {
            par_val.push_back(core::parameter::annotation::parameterToVector<bool>(
                {static_cast<bool>(values.at(idx_i))}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
        if (par_type.at(idx_i) == core::DataType::UINT16) {
            par_val.push_back(core::parameter::annotation::parameterToVector<uint16_t>(
                {static_cast<uint16_t>(values.at(idx_i))}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
        if (par_type.at(idx_i) == core::DataType::STRING) {
            par_val.push_back(core::parameter::annotation::parameterToVector<uint8_t>(
                {0}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
    }

    return genie::core::parameter::annotation::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

genie::core::parameter::annotation::CompressorParameterSet ZSTDParameters::compressorParameterSet(
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

}  // namespace genie::entropy::zstd

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

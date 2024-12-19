/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX  // NOLINT
#include "genie/entropy/gabac/benchmark.h"

#include <algorithm>
#include <filesystem>  // NOLINT
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/entropy/gabac/encode_desc_sub_seq.h"
#include "genie/entropy/gabac/encode_transformed_sub_seq.h"
#include "genie/entropy/gabac/stream_handler.h"
#include "genie/util/log.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "App/Gabac";

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
paramcabac::Binarization ConfigSearchBinarization::GetBinarization(
    const bool bypass, const uint8_t output_bits,
    const uint8_t sub_symbol_size) const {
  return paramcabac::Binarization(
      lut_[binarization_search_idx_], bypass,
      paramcabac::BinarizationParameters(
          lut_[binarization_search_idx_],
          {binarization_parameters_[binarization_search_idx_].GetIndex(
              binarization_parameter_search_idx_)}),
      paramcabac::Context(true, output_bits, sub_symbol_size, true));
}

// -----------------------------------------------------------------------------
ConfigSearchBinarization::ConfigSearchBinarization(
    const std::pair<int64_t, int64_t>& range, uint8_t split_size)
    : binarization_search_idx_(0), binarization_parameter_search_idx_(0) {
  auto bits = static_cast<uint8_t>(std::ceil(
      std::log2(std::max(std::abs(range.first), std::abs(range.second)))));
  if (range.first < 0) {
    bits++;
  }
  {
    if (split_size == 0) {
      lut_.emplace_back(paramcabac::BinarizationParameters::BinarizationId::BI);
      binarization_parameters_.emplace_back(bits, bits,
                                            static_cast<uint8_t>(1));
    }
  }
  if (range.first >= 0) {
    if (split_size != 0 && split_size != bits) {
      lut_.emplace_back(
          paramcabac::BinarizationParameters::BinarizationId::SUTU);
      binarization_parameters_.emplace_back(split_size, split_size,
                                            static_cast<uint8_t>(1));
    } else {
      if (range.second < 256) {
        lut_.emplace_back(
            paramcabac::BinarizationParameters::BinarizationId::TU);
        binarization_parameters_.emplace_back(
            static_cast<uint8_t>(range.second),
            static_cast<uint8_t>(range.second), static_cast<uint8_t>(1));
      }
      lut_.emplace_back(paramcabac::BinarizationParameters::BinarizationId::EG);
      binarization_parameters_.emplace_back(static_cast<uint8_t>(0),
                                            static_cast<uint8_t>(0),
                                            static_cast<uint8_t>(1));
    }
  } else {
    if (split_size != 0 && split_size != bits) {
      lut_.emplace_back(
          paramcabac::BinarizationParameters::BinarizationId::SSUTU);
      binarization_parameters_.emplace_back(split_size, split_size,
                                            static_cast<uint8_t>(1));
    } else {
      lut_.emplace_back(
          paramcabac::BinarizationParameters::BinarizationId::SEG);
      binarization_parameters_.emplace_back(static_cast<uint8_t>(0),
                                            static_cast<uint8_t>(0),
                                            static_cast<uint8_t>(1));
    }
  }
  binarization_ =
      SearchSpace<uint8_t>(0, static_cast<uint8_t>(lut_.size()) - 1, 1);
}

// -----------------------------------------------------------------------------
bool ConfigSearchBinarization::Increment() {
  binarization_parameter_search_idx_++;
  if (binarization_parameter_search_idx_ ==
      binarization_parameters_[binarization_search_idx_].Size()) {
    binarization_parameter_search_idx_ = 0;
    binarization_search_idx_++;
  }
  if (binarization_search_idx_ == binarization_.Size()) {
    binarization_search_idx_ = 0;
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
void ConfigSearchBinarization::Mutate(std::mt19937& rd,
                                      std::normal_distribution<>& d) {
  binarization_search_idx_ =
      binarization_.Mutate(binarization_search_idx_, static_cast<float>(d(rd)));
  binarization_parameter_search_idx_ =
      binarization_parameters_[binarization_search_idx_].Mutate(
          binarization_parameter_search_idx_, static_cast<float>(d(rd)));
}

// -----------------------------------------------------------------------------
bool ConfigSearchTransformedSeq::LutValid() const {
  return output_bits_ / GetSplitRatio() <= 8 &&
         coding_order_.GetIndex(coding_order_search_idx_) > 0;
}

// -----------------------------------------------------------------------------

int8_t ConfigSearchTransformedSeq::GetSubSymbolTransId() const {
  if (subsymbol_transform_enabled_.GetIndex(subsymbol_transform_search_idx_) ==
      0) {
    return 0;
  }
  if (coding_order_.GetIndex(coding_order_search_idx_) == 0) {
    return 0;
  }
  return 2;
}

// -----------------------------------------------------------------------------

bool ConfigSearchTransformedSeq::IncrementTransform() {
  subsymbol_transform_search_idx_++;
  if (subsymbol_transform_search_idx_ == subsymbol_transform_enabled_.Size() ||
      (GetSubSymbolTransId() == 2 && !LutValid())) {
    subsymbol_transform_search_idx_ = 0;
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------

uint8_t ConfigSearchTransformedSeq::GetSplitRatio() const {
  return 1 << split_size_.GetIndex(split_size_idx_);
}

// -----------------------------------------------------------------------------

paramcabac::TransformedSubSeq ConfigSearchTransformedSeq::CreateConfig(
    const core::GenSubIndex& descriptor_subsequence,
    const bool original) const {
  int lut[] = {0, 2, 1};
  const auto sub_symbol_trans =
      static_cast<paramcabac::SupportValues::TransformIdSubsym>(
          lut[GetSubSymbolTransId()]);

  auto symbol_size = output_bits_;
  if (!split_in_binarization_.GetIndex(split_in_binarization_idx_)) {
    symbol_size = output_bits_ / GetSplitRatio();
  }

  paramcabac::TransformedSubSeq ret(
      sub_symbol_trans,
      paramcabac::SupportValues(
          output_bits_, symbol_size,
          coding_order_.GetIndex(coding_order_search_idx_)),
      binarizations_[split_size_.GetIndex(split_size_idx_)]
                    [split_in_binarization_.GetIndex(
                        split_in_binarization_idx_)][GetSubSymbolTransId()]
                        .GetBinarization(bypass_.GetIndex(bypass_search_idx_),
                                         output_bits_, symbol_size),
      descriptor_subsequence, original);
  return ret;
}

// -----------------------------------------------------------------------------

bool ConfigSearchTransformedSeq::Increment() {  // NOLINT
  // Increment binarization for current split and transformation
  if (binarizations_[split_size_.GetIndex(split_size_idx_)]
                    [split_in_binarization_.GetIndex(
                        split_in_binarization_idx_)][GetSubSymbolTransId()]
                        .Increment()) {
    return true;
  }

  // Increment sub symbol transformation
  if (IncrementTransform()) {
    return true;
  }

  split_in_binarization_idx_++;
  if (split_in_binarization_idx_ == split_in_binarization_.Size()) {
    split_in_binarization_idx_ = 0;
    split_size_idx_++;
  }

  if (split_size_idx_ == split_size_.Size()) {
    split_size_idx_ = 0;
    bypass_search_idx_++;
  }

  // Increment bypass setting
  if (bypass_search_idx_ == bypass_.Size()) {
    bypass_search_idx_ = 0;
    coding_order_search_idx_++;
  }

  // If coding order cannot be incremented, all settings have been explored
  if (coding_order_search_idx_ == coding_order_.Size()) {
    coding_order_search_idx_ = 0;
    return false;
  }

  // Skip split binarizations for coding order > 0
  while ((coding_order_.GetIndex(coding_order_search_idx_) > 0 &&
          split_in_binarization_.GetIndex(split_in_binarization_idx_)) ||
         output_bits_ % GetSplitRatio() ||
         ((coding_order_.GetIndex(coding_order_search_idx_) == 2 &&
           output_bits_ / GetSplitRatio() > 8) ||
          (coding_order_.GetIndex(coding_order_search_idx_) == 1 &&
           output_bits_ / GetSplitRatio() > 16))) {
    if (!Increment()) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------

ConfigSearchTransformedSeq::ConfigSearchTransformedSeq(
    const std::pair<int64_t, int64_t>& range)
    : split_size_(0, 3, 1),
      split_size_idx_(0),
      split_in_binarization_(0, 0, 1),
      split_in_binarization_idx_(0),
      coding_order_(0, 2, 1),
      coding_order_search_idx_(0),
      bypass_(0, 0, 1),
      bypass_search_idx_(0),
      subsymbol_transform_enabled_(0, 1, 1),
      subsymbol_transform_search_idx_(0) {
  // Necessary bits
  output_bits_ = static_cast<uint8_t>(std::ceil(log2(static_cast<double>(
      std::max(std::abs(range.first) + 1, std::abs(range.second) + 1)))));
  if (range.first < 0) {
    output_bits_++;
  }
  // Split sizes 2^0 to 2^4
  for (int i = 0; i < 4; ++i) {
    binarizations_.emplace_back();
    // Split at binarization level (1) or as sub symbol (0)
    for (int j = 0; j < 2; ++j) {
      binarizations_.back().emplace_back();

      const uint8_t split_size = static_cast<uint8_t>(1) << i;
      uint8_t coding_size = output_bits_ / split_size;

      if (j == 0) {
        coding_size = 0;
      }
      binarizations_.back().back().emplace_back(range,
                                                coding_size);  // No-Transform
      binarizations_.back().back().emplace_back(
          std::make_pair(-(range.second - range.first),
                         range.second - range.first),
          coding_size);  // DIFF
      binarizations_.back().back().emplace_back(
          std::make_pair(0, range.second - range.first),
          coding_size);  // LUT
    }
  }
}

// -----------------------------------------------------------------------------

void ConfigSearchTransformedSeq::Mutate(std::mt19937& rd,
                                        std::normal_distribution<>& d) {
  coding_order_search_idx_ =
      static_cast<size_t>(static_cast<unsigned char>(coding_order_.Mutate(
          coding_order_search_idx_, static_cast<float>(d(rd)))));
  split_size_idx_ = static_cast<size_t>(static_cast<unsigned char>(
      split_size_.Mutate(split_size_idx_, static_cast<float>(d(rd)))));
  subsymbol_transform_search_idx_ = static_cast<size_t>(
      static_cast<unsigned char>(subsymbol_transform_enabled_.Mutate(
          subsymbol_transform_search_idx_, static_cast<float>(d(rd)))));
  bypass_search_idx_ = static_cast<size_t>(static_cast<unsigned char>(
      bypass_.Mutate(bypass_search_idx_, static_cast<float>(d(rd)))));
  if (GetSubSymbolTransId() == 2 && !LutValid()) {
    subsymbol_transform_search_idx_ = 0;
  }
  binarizations_[split_size_.GetIndex(split_size_idx_)]
                [split_in_binarization_.GetIndex(split_in_binarization_idx_)]
                [GetSubSymbolTransId()]
                    .Mutate(rd, d);
}

/**
 * @brief
 * @return
 */
std::string ResultTransformed::ToCsv(const std::string& filename,
                                     const size_t transform,
                                     const size_t parameter,
                                     const size_t seq_id) const {
  std::string ret;
  ret += filename + ";" + std::to_string(size) + ";" +
         std::to_string(milliseconds) + ";" + std::to_string(transform) + ";" +
         std::to_string(parameter) + ";" + std::to_string(seq_id) + ";";
  ret +=
      std::to_string(config.GetSupportValues().GetCodingOrder()) + ";" +
      std::to_string(config.GetSupportValues().GetOutputSymbolSize()) + ";" +
      std::to_string(config.GetSupportValues().GetCodingSubsymSize()) + ";" +
      std::to_string(static_cast<size_t>(config.GetTransformIdSubsym())) + ";" +
      std::to_string(
          static_cast<size_t>(config.GetBinarization().GetBinarizationId())) +
      ";";
  if (config.GetBinarization().GetBinarizationId() ==
      paramcabac::BinarizationParameters::BinarizationId::TU) {
    ret +=
        std::to_string(static_cast<size_t>(config.GetBinarization()
                                               .GetCabacBinarizationParameters()
                                               .GetCMax())) +
        ";";
  } else if (config.GetBinarization().GetBinarizationId() ==
             paramcabac::BinarizationParameters::BinarizationId::SUTU) {
    ret +=
        std::to_string(static_cast<size_t>(config.GetBinarization()
                                               .GetCabacBinarizationParameters()
                                               .GetSplitUnitSize())) +
        ";";
  } else {
    ret += ";";
  }
  return ret;
}

/**
 * @brief
 * @return
 */
std::string ResultTransformed::GetCsvHeader() {
  std::string desc;
  desc +=
      "File;Compressed Size;Compression "
      "time;Transformation;TransformationParam;seqID;";
  desc +=
      "Coding order;Symbol size;Sub symbol Size;Sub symbol "
      "transformation;Binarization;BinParam;";
  return desc;
}

// -----------------------------------------------------------------------------

std::string ResultFull::ToCsv(const std::string& filename) const {
  std::string ret;
  ret += std::to_string(size) + ";" + std::to_string(milliseconds) + ";" +
         std::to_string(static_cast<size_t>(
             config.GetTransformParameters().GetTransformIdSubseq())) +
         ";" +
         std::to_string(
             static_cast<size_t>(config.GetTransformParameters().GetParam())) +
         ";";
  for (auto& cfg : config.GetTransformSubSeqConfigs()) {
    ret += std::to_string(cfg.GetSupportValues().GetCodingOrder()) + ";" +
           std::to_string(cfg.GetSupportValues().GetOutputSymbolSize()) + ";" +
           std::to_string(cfg.GetSupportValues().GetCodingSubsymSize()) + ";" +
           std::to_string(static_cast<size_t>(cfg.GetTransformIdSubsym())) +
           ";" +
           std::to_string(
               static_cast<size_t>(cfg.GetBinarization().GetBinarizationId())) +
           ";";
    if (cfg.GetBinarization().GetBinarizationId() ==
        paramcabac::BinarizationParameters::BinarizationId::TU) {
      ret += std::to_string(
                 static_cast<size_t>(cfg.GetBinarization()
                                         .GetCabacBinarizationParameters()
                                         .GetCMax())) +
             ";";
    } else if (cfg.GetBinarization().GetBinarizationId() ==
               paramcabac::BinarizationParameters::BinarizationId::SUTU) {
      ret += std::to_string(
                 static_cast<size_t>(cfg.GetBinarization()
                                         .GetCabacBinarizationParameters()
                                         .GetSplitUnitSize())) +
             ";";
    } else {
      ret += ";";
    }
  }
  for (int i = 0;
       i < 3 - static_cast<int>(config.GetTransformSubSeqConfigs().size());
       ++i) {
    ret += ";;;;;;";
  }
  return filename + ";" + ret;
}

// -----------------------------------------------------------------------------

std::string ResultFull::GetCsvHeader() const {
  std::string desc;
  desc +=
      "File;Compressed Size;Compression "
      "time;Transformation;TransformationParam;";
  for (size_t i = 0; i < config.GetTransformSubSeqConfigs().size(); ++i) {
    desc +=
        "Coding order;Symbol size;Sub symbol Size;Sub symbol "
        "transformation;Binarization;BinParam;";
  }
  for (int i = 0;
       i < 3 - static_cast<int>(config.GetTransformSubSeqConfigs().size());
       ++i) {
    desc +=
        "Coding order;Symbol size;Sub symbol Size;Sub symbol "
        "transformation;Binarization;BinParam;";
  }
  return desc;
}

// -----------------------------------------------------------------------------

ResultFull BenchmarkFull(const std::string& input_file,
                         const core::GenSubIndex& desc, float time_weight) {
  std::ifstream input_stream(input_file);
  UTILS_DIE_IF(!input_stream, "Cannot open file to read: " + input_file);
  util::DataBlock sequence(0, core::Range2Bytes(GetSubsequence(desc).range));
  StreamHandler::ReadFull(input_stream, &sequence);

  ResultFull best_result;
  float best_score = INFINITY;
  ConfigSearch config(GetSubsequence(desc).range);
  bool new_file = !std::filesystem::exists("benchmark_total.csv");
  std::ofstream results_file("benchmark_total.csv", std::ios_base::app);
  do {
    UTILS_LOG(
        util::Logger::Severity::INFO,
        "Optimizing transformation " + std::to_string(config.GetTransform()));
    // Execute transformation
    std::vector<util::DataBlock> transformed_sub_seqs;
    transformed_sub_seqs.resize(1);
    auto subsequence = sequence;
    transformed_sub_seqs[0].Swap(&subsequence);
    util::Watch timer;
    timer.Reset();
    auto cfg = config.CreateConfig(desc, GetDescriptor(desc.first).token_type);
    DoSubsequenceTransform(cfg, &transformed_sub_seqs);
    auto total_time = static_cast<size_t>(timer.Check() * 1000);
    std::vector<ResultTransformed> trans_results;

    // Optimize transformed sequences independently
    for (size_t i = 0; i < transformed_sub_seqs.size(); ++i) {
      UTILS_LOG(util::Logger::Severity::INFO,
                "Optimizing subsequence " + std::to_string(i) + "...");
      trans_results.emplace_back(OptimizeTransformedSequence(
          config.GetTransformedSeqs()[i], desc, transformed_sub_seqs[i],
          time_weight, i == transformed_sub_seqs.size() - 1,
          config.GetTransform(), config.GetTransformParam(), i, input_file));
      auto tmp = trans_results.back().config;
      cfg.SetTransformSubSeqCfg(i, std::move(tmp));
    }

    // Sum up compressed sizes and times
    size_t total_size = 0;
    for (const auto& r : trans_results) {
      total_size += r.size;
      total_time += r.milliseconds;  // Transformation time already added above
    }

    // Calculate weighted score

    // Found new best score
    if (float score = time_weight * static_cast<float>(total_time) +
                      (1 - time_weight) * static_cast<float>(total_size);
        score < best_score) {
      best_score = score;
      best_result.milliseconds = total_time;
      best_result.size = total_size;
      best_result.config = cfg;
    }

    ResultFull result_current;
    result_current.milliseconds = total_time;
    result_current.size = total_size;
    result_current.config = cfg;

    if (new_file) {
      results_file << result_current.GetCsvHeader() << std::endl;
      new_file = false;
    }
    results_file << result_current.ToCsv(input_file) << std::endl;
  } while (config.Increment());

  std::cout << best_result.ToCsv(input_file) << std::endl;
  return best_result;
}

// -----------------------------------------------------------------------------

int16_t ConfigSearchTransformation::GetParameter() const {
  return parameter_.GetIndex(parameter_search_idx_);
}

// -----------------------------------------------------------------------------

ConfigSearchTransformation::ConfigSearchTransformation(
    const paramcabac::TransformedParameters::TransformIdSubseq trans_id,
    const std::pair<int64_t, int64_t>& range)
    : parameter_search_idx_(0) {
  constexpr uint16_t match_coding_buffer_size = 255;
  constexpr uint8_t rle_guard = 255;
  switch (trans_id) {
    case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
      parameter_ = SearchSpace<int16_t>(0, 0, 1);  // No param
      transformed_seqs_.emplace_back(range);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
      parameter_ = SearchSpace<int16_t>(0, 0, 1);  // No param
      transformed_seqs_.emplace_back(std::make_pair(0, 1));
      transformed_seqs_.emplace_back(range);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
      parameter_ = SearchSpace<int16_t>(match_coding_buffer_size,
                                        match_coding_buffer_size, 1);
      transformed_seqs_.emplace_back(
          std::make_pair(0, match_coding_buffer_size));
      transformed_seqs_.emplace_back(
          std::make_pair(0, match_coding_buffer_size));
      transformed_seqs_.emplace_back(range);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
      parameter_ = SearchSpace<int16_t>(rle_guard, rle_guard, 1);
      transformed_seqs_.emplace_back(std::make_pair(0, rle_guard));
      transformed_seqs_.emplace_back(range);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
      UTILS_DIE("Merge coding unsupported");
  }
}

// -----------------------------------------------------------------------------

std::vector<ConfigSearchTransformedSeq>&
ConfigSearchTransformation::GetTransformedSeqs() {
  return transformed_seqs_;
}

// -----------------------------------------------------------------------------

bool ConfigSearchTransformation::Increment() {
  // Try all parameters
  parameter_search_idx_++;
  if (parameter_search_idx_ == parameter_.Size()) {
    // All parameters tried
    parameter_search_idx_ = 0;
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------

void ConfigSearchTransformation::Mutate(std::mt19937& rd,
                                        std::normal_distribution<>& d) {
  parameter_search_idx_ =
      parameter_.Mutate(parameter_search_idx_, static_cast<float>(d(rd)));

  for (auto& ts : transformed_seqs_) {
    ts.Mutate(rd, d);
  }
}

// -----------------------------------------------------------------------------

uint8_t ConfigSearch::GetTransform() const {
  return transformation_.GetIndex(transformation_search_idx_);
}

// -----------------------------------------------------------------------------

uint32_t ConfigSearch::GetTransformParam() const {
  return params_[transformation_.GetIndex(transformation_search_idx_)]
      .GetParameter();
}

// -----------------------------------------------------------------------------

std::vector<ConfigSearchTransformedSeq>& ConfigSearch::GetTransformedSeqs() {
  return params_[transformation_.GetIndex(transformation_search_idx_)]
      .GetTransformedSeqs();
}

// -----------------------------------------------------------------------------

ConfigSearch::ConfigSearch(const std::pair<int64_t, int64_t>& range)
    : transformation_search_idx_(0) {
  params_.emplace_back(
      paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM,
      range);
  params_.emplace_back(
      paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING,
      range);
  params_.emplace_back(
      paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING,
      range);
  params_.emplace_back(
      paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING, range);
  transformation_ = SearchSpace<int8_t>(0, 3, 1);
}

// -----------------------------------------------------------------------------

bool ConfigSearch::Increment() {
  // Search current transformation
  if (params_[transformation_.GetIndex(transformation_search_idx_)]
          .Increment()) {
    return true;
  }

  // Current transformation completely searched, next transformation
  transformation_search_idx_++;
  if (transformation_search_idx_ == transformation_.Size()) {
    transformation_search_idx_ = 0;
    return false;
  }

  // All transformations searched
  return true;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void ConfigSearch::Mutate(const float rate) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::normal_distribution<> d{0, rate};

  transformation_search_idx_ =
      static_cast<size_t>(static_cast<unsigned char>(transformation_.Mutate(
          transformation_search_idx_, static_cast<float>(d(gen)))));
  params_[transformation_.GetIndex(transformation_search_idx_)].Mutate(gen, d);
}

// -----------------------------------------------------------------------------

paramcabac::Subsequence ConfigSearch::CreateConfig(
    const core::GenSubIndex& descriptor_subsequence, const bool token_type) {
  // Generate top level transformation
  auto t = paramcabac::TransformedParameters(
      static_cast<paramcabac::TransformedParameters::TransformIdSubseq>(
          transformation_.GetIndex(transformation_search_idx_)),
      params_[transformation_.GetIndex(transformation_search_idx_)]
          .GetParameter());

  // Add config for transformed sequences
  std::vector<paramcabac::TransformedSubSeq> tss;
  for (auto& p : params_[transformation_.GetIndex(transformation_search_idx_)]
                     .GetTransformedSeqs()) {
    constexpr size_t i = 0;
    tss.emplace_back(p.CreateConfig(
        descriptor_subsequence,
        i == params_[transformation_.GetIndex(transformation_search_idx_)]
                     .GetTransformedSeqs()
                     .size() -
                 1));
  }
  paramcabac::Subsequence ret(std::move(t), descriptor_subsequence.second,
                              token_type, std::move(tss));
  return ret;
}

// -----------------------------------------------------------------------------

ResultTransformed OptimizeTransformedSequence(
    ConfigSearchTransformedSeq& seq, const core::GenSubIndex& sub_sequence,
    util::DataBlock& data, float time_weight, bool original,
    size_t transformation, size_t transformation_param, size_t sequence_id,
    const std::string& filename) {
  ResultTransformed ret;
  float score = INFINITY;
  bool new_file = !std::filesystem::exists(
      "benchmark_trans_" + std::to_string(transformation) + ".csv");
  std::ofstream results_file(
      "benchmark_trans_" + std::to_string(transformation) + ".csv",
      std::ios_base::app);
  do {
    auto cfg = seq.CreateConfig(sub_sequence, original);
    auto input = data;

    util::Watch watch;
    watch.Reset();
    EncodeTransformSubSeq(cfg, &input);
    auto time = static_cast<size_t>(watch.Check() * 1000);  // Milliseconds
    auto size = input.GetRawSize();

    if (auto new_score =
            static_cast<float>(time_weight * static_cast<float>(time) +
                               (1.0 - time_weight) * static_cast<float>(size));
        new_score < score) {
      // Found new bets score
      score = new_score;
      ret.milliseconds = time;
      ret.size = size;
      ret.config = cfg;
    }
    ResultTransformed result_current;
    result_current.milliseconds = time;
    result_current.size = size;
    result_current.config = cfg;

    if (new_file) {
      results_file << ResultTransformed::GetCsvHeader() << std::endl;
      new_file = false;
    }
    results_file << result_current.ToCsv(filename, transformation,
                                         transformation_param, sequence_id)
                 << std::endl;
  } while (seq.Increment());
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

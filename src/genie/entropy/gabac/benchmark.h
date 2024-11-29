/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BENCHMARK_H_
#define SRC_GENIE_ENTROPY_GABAC_BENCHMARK_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/entropy/paramcabac/transformed_sub_seq.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 * @tparam Type
 */
template <typename Type>
class SearchSpace {
 public:
  /**
   * @brief
   */
  class SearchSpaceIterator {
    Type value_;   //!< @brief
    Type stride_;  //!< @brief

   public:
    /**
     * @brief
     * @param value
     * @param stride
     */
    SearchSpaceIterator(Type value, Type stride);

    /**
     * @brief
     * @tparam Ret
     * @return
     */
    template <typename Ret>
    Ret Get() const;

    /**
     * @brief
     * @param idx
     * @return
     */
    SearchSpaceIterator& operator+=(int64_t idx);

    /**
     * @brief
     * @param idx
     * @return
     */
    SearchSpaceIterator& operator-=(int64_t idx);

    /**
     * @brief
     * @param idx
     * @return
     */
    SearchSpaceIterator operator+(int64_t idx) const;

    /**
     * @brief
     * @param idx
     * @return
     */
    SearchSpaceIterator operator-(int64_t idx) const;

    /**
     * @brief
     * @param other
     * @return
     */
    int64_t operator-(SearchSpaceIterator& other) const;

    /**
     * @brief
     * @return
     */
    SearchSpaceIterator& operator++();

    /**
     * @brief
     * @return
     */
    SearchSpaceIterator& operator--();

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const SearchSpaceIterator& other) const;

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator!=(const SearchSpaceIterator& other) const;
  };

 private:
  Type min_;     //!< @brief
  Type max_;     //!< @brief
  Type stride_;  //!< @brief

 public:
  /**
   * @brief
   */
  SearchSpace();

  /**
   * @brief
   * @param min
   * @param max
   * @param stride
   */
  SearchSpace(Type min, Type max, Type stride);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] SearchSpaceIterator begin() const;  // NOLINT

  /**
   * @brief
   * @return
   */
  [[nodiscard]] SearchSpaceIterator end() const;  // NOLINT

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t Size() const;

  /**
   * @brief
   * @param idx
   * @return
   */
  [[nodiscard]] Type GetIndex(size_t idx) const;

  /**
   * @brief
   * @param index
   * @param random
   * @return
   */
  Type Mutate(size_t index, float random);
};

/**
 * @brief
 */
class ConfigSearchBinarization {
  SearchSpace<uint8_t> binarization_;  //!< @brief
  size_t binarization_search_idx_;     //!< @brief
  std::vector<paramcabac::BinarizationParameters::BinarizationId>
      lut_;  //!< @brief

  std::vector<SearchSpace<uint8_t>> binarization_parameters_;  //!< @brief
  size_t binarization_parameter_search_idx_;                   //!< @brief

 public:
  /**
   * @brief
   * @param bypass
   * @param output_bits
   * @param sub_symbol_size
   * @return
   */
  [[nodiscard]] paramcabac::Binarization GetBinarization(
      bool bypass, uint8_t output_bits, uint8_t sub_symbol_size) const;

  /**
   * @brief
   * @param range
   * @param split_size
   */
  explicit ConfigSearchBinarization(const std::pair<int64_t, int64_t>& range,
                                    uint8_t split_size);

  /**
   * @brief
   * @return
   */
  bool Increment();

  /**
   * @brief
   * @param rd
   * @param d
   */
  void Mutate(std::mt19937& rd, std::normal_distribution<>& d);
};

/**
 * @brief
 */
class ConfigSearchTransformedSeq {
  uint8_t output_bits_;  //!< @brief

  SearchSpace<int8_t> split_size_;  //!< @brief
  size_t split_size_idx_;           //!< @brief

  SearchSpace<int8_t> split_in_binarization_;  //!< @brief
  size_t split_in_binarization_idx_;           //!< @brief

  SearchSpace<int8_t> coding_order_;  //!< @brief
  size_t coding_order_search_idx_;    //!< @brief

  SearchSpace<int8_t> bypass_;  //!< @brief
  size_t bypass_search_idx_;    //!< @brief

  SearchSpace<int8_t> subsymbol_transform_enabled_;  //!< @brief
  size_t subsymbol_transform_search_idx_;            //!< @brief
  std::vector<std::vector<std::vector<ConfigSearchBinarization>>>
      binarizations_;  //!< @brief

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool LutValid() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] int8_t GetSubSymbolTransId() const;

  /**
   * @brief
   * @return
   */
  bool IncrementTransform();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetSplitRatio() const;

 public:
  /**
   * @brief
   * @param descriptor_subsequence
   * @param original
   * @return
   */
  [[nodiscard]] paramcabac::TransformedSubSeq CreateConfig(
      const core::GenSubIndex& descriptor_subsequence, bool original) const;

  /**
   * @brief
   * @return
   */
  bool Increment();

  /**
   * @brief
   * @param range
   */
  explicit ConfigSearchTransformedSeq(const std::pair<int64_t, int64_t>& range);

  /**
   * @brief
   * @param rd
   * @param d
   */
  void Mutate(std::mt19937& rd, std::normal_distribution<>& d);
};

/**
 * @brief
 */
struct ResultTransformed {
  size_t milliseconds{};                 //!< @brief
  size_t size{};                         //!< @brief
  paramcabac::TransformedSubSeq config;  //!< @brief

  /**
   * @brief
   * @param filename
   * @param transform
   * @param parameter
   * @param seq_id
   * @return
   */
  [[nodiscard]] std::string ToCsv(const std::string& filename, size_t transform,
                                  size_t parameter, size_t seq_id) const;

  /**
   * @brief
   * @return
   */
  static std::string GetCsvHeader();
};

/**
 * @brief
 */
struct ResultFull {
  size_t milliseconds{};           //!< @brief
  size_t size{};                   //!< @brief
  paramcabac::Subsequence config;  //!< @brief

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string ToCsv(const std::string& filename) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string GetCsvHeader() const;
};

/**
 * @brief
 * @param seq
 * @param sub_sequence
 * @param data
 * @param time_weight
 * @param original
 * @param transformation
 * @param transformation_param
 * @param sequence_id
 * @param filename
 * @return
 */
ResultTransformed OptimizeTransformedSequence(
    ConfigSearchTransformedSeq& seq, const core::GenSubIndex& sub_sequence,
    util::DataBlock& data, float time_weight, bool original,
    size_t transformation, size_t transformation_param, size_t sequence_id,
    const std::string& filename);

/**
 * @brief
 * @param input_file
 * @param desc
 * @param time_weight
 * @return
 */
ResultFull BenchmarkFull(const std::string& input_file,
                         const core::GenSubIndex& desc, float time_weight);

/**
 * @brief
 */
class ConfigSearchTransformation {
  SearchSpace<int16_t> parameter_;  //!< @brief
  size_t parameter_search_idx_;     //!< @brief

  std::vector<ConfigSearchTransformedSeq> transformed_seqs_;  //!< @brief

 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] int16_t GetParameter() const;

  /**
   * @brief
   * @param trans_id
   * @param range
   */
  ConfigSearchTransformation(
      paramcabac::TransformedParameters::TransformIdSubseq trans_id,
      const std::pair<int64_t, int64_t>& range);

  /**
   * @brief
   * @return
   */
  std::vector<ConfigSearchTransformedSeq>& GetTransformedSeqs();

  /**
   * @brief
   * @return
   */
  bool Increment();

  /**
   * @brief
   * @param rd
   * @param d
   */
  void Mutate(std::mt19937& rd, std::normal_distribution<>& d);
};

/**
 * @brief
 */
class ConfigSearch {
  SearchSpace<int8_t> transformation_;              //!< @brief
  size_t transformation_search_idx_;                //!< @brief
  std::vector<ConfigSearchTransformation> params_;  //!< @brief

 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetTransform() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetTransformParam() const;

  /**
   * @brief
   * @return
   */
  std::vector<ConfigSearchTransformedSeq>& GetTransformedSeqs();

  /**
   * @brief
   * @param range
   */
  explicit ConfigSearch(const std::pair<int64_t, int64_t>& range);

  /**
   * @brief
   * @return
   */
  bool Increment();

  /**
   * @brief
   * @param rate
   */
  [[maybe_unused]] void Mutate(float rate);

  /**
   * @brief
   * @param descriptor_subsequence
   * @param token_type
   * @return
   */
  paramcabac::Subsequence CreateConfig(
      const core::GenSubIndex& descriptor_subsequence, bool token_type);
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#include "genie/entropy/gabac/benchmark.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BENCHMARK_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

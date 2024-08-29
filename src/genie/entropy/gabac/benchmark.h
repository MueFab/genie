/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BENCHMARK_H_
#define SRC_GENIE_ENTROPY_GABAC_BENCHMARK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/entropy/paramcabac/transformed-subseq.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 * @tparam TYPE
 */
template <typename TYPE>
class SearchSpace {
 public:
    /**
     * @brief
     */
    class SearchSpaceIterator {
     private:
        TYPE value;   //!< @brief
        TYPE stride;  //!< @brief

     public:
        /**
         * @brief
         * @param _value
         * @param _stride
         */
        SearchSpaceIterator(TYPE _value, TYPE _stride);

        /**
         * @brief
         * @tparam RET
         * @return
         */
        template <typename RET>
        RET get() const;

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
    TYPE min;     //!< @brief
    TYPE max;     //!< @brief
    TYPE stride;  //!< @brief

 public:
    /**
     * @brief
     */
    SearchSpace();

    /**
     * @brief
     * @param _min
     * @param _max
     * @param _stride
     */
    SearchSpace(TYPE _min, TYPE _max, TYPE _stride);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] SearchSpaceIterator begin() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] SearchSpaceIterator end() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief
     * @param idx
     * @return
     */
    [[nodiscard]] TYPE getIndex(size_t idx) const;

    /**
     * @brief
     * @param index
     * @param random
     * @return
     */
    TYPE mutate(size_t index, float random);
};

/**
 * @brief
 */
class ConfigSearchBinarization {
 private:
    SearchSpace<uint8_t> binarization;                                                    //!< @brief
    size_t binarization_search_idx;                                                       //!< @brief
    std::vector<genie::entropy::paramcabac::BinarizationParameters::BinarizationId> lut;  //!< @brief

    std::vector<SearchSpace<uint8_t>> binarizationParameters;  //!< @brief
    size_t binarization_parameter_search_idx;                  //!< @brief

 public:
    /**
     * @brief
     * @param bypass
     * @param outputbits
     * @param subsymsize
     * @return
     */
    [[nodiscard]] paramcabac::Binarization getBinarization(bool bypass, uint8_t outputbits, uint8_t subsymsize) const;

    /**
     * @brief
     * @param range
     * @param splitsize
     */
    explicit ConfigSearchBinarization(const std::pair<int64_t, int64_t>& range, uint8_t splitsize);

    /**
     * @brief
     * @return
     */
    bool increment();

    /**
     * @brief
     * @param rd
     * @param d
     */
    void mutate(std::mt19937& rd, std::normal_distribution<>& d);
};

/**
 * @brief
 */
class ConfigSearchTranformedSeq {
 private:
    uint8_t output_bits;  //!< @brief

    SearchSpace<int8_t> split_size;  //!< @brief
    size_t split_size_idx;           //!< @brief

    SearchSpace<int8_t> split_in_binarization;  //!< @brief
    size_t split_in_binarization_idx;           //!< @brief

    SearchSpace<int8_t> coding_order;  //!< @brief
    size_t coding_order_search_idx;    //!< @brief

    SearchSpace<int8_t> bypass;  //!< @brief
    size_t bypass_search_idx;    //!< @brief

    SearchSpace<int8_t> subsymbol_transform_enabled;                                //!< @brief
    size_t subsymbol_transform_search_idx;                                          //!< @brief
    std::vector<std::vector<std::vector<ConfigSearchBinarization>>> binarizations;  //!< @brief

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool lutValid() const;

    /**
     * @brief
     * @return
     */
    int8_t getSubsymbolTransID();

    /**
     * @brief
     * @return
     */
    bool incrementTransform();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getSplitRatio() const;

 public:
    /**
     * @brief
     * @param descriptor_subsequence
     * @param original
     * @return
     */
    paramcabac::TransformedSubSeq createConfig(const genie::core::GenSubIndex& descriptor_subsequence, bool original);

    /**
     * @brief
     * @return
     */
    bool increment();

    /**
     * @brief
     * @param range
     */
    explicit ConfigSearchTranformedSeq(const std::pair<int64_t, int64_t>& range);

    /**
     * @brief
     * @param rd
     * @param d
     */
    void mutate(std::mt19937& rd, std::normal_distribution<>& d);
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
    [[nodiscard]] std::string toCSV(const std::string& filename, size_t transform, size_t parameter,
                                    size_t seq_id) const;

    /**
     * @brief
     * @return
     */
    static std::string getCSVHeader();
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
    [[nodiscard]] std::string toCSV(const std::string& filename) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::string getCSVHeader() const;
};

/**
 * @brief
 * @param seq
 * @param gensub
 * @param data
 * @param timeweight
 * @param original
 * @param transformation
 * @param transformation_param
 * @param sequence_id
 * @param filename
 * @return
 */
ResultTransformed optimizeTransformedSequence(ConfigSearchTranformedSeq& seq, const genie::core::GenSubIndex& gensub,
                                              util::DataBlock& data, float timeweight, bool original,
                                              size_t transformation, size_t transformation_param, size_t sequence_id,
                                              const std::string& filename);

/**
 * @brief
 * @param input_file
 * @param desc
 * @param timeweight
 * @return
 */
ResultFull benchmark_full(const std::string& input_file, const genie::core::GenSubIndex& desc, float timeweight);

/**
 * @brief
 */
class ConfigSearchTransformation {
 private:
    SearchSpace<int16_t> parameter;  //!< @brief
    size_t parameter_search_idx;     //!< @brief

    std::vector<ConfigSearchTranformedSeq> transformedSeqs;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    [[nodiscard]] int16_t getParameter() const;

    /**
     * @brief
     * @param trans_id
     * @param range
     */
    ConfigSearchTransformation(paramcabac::TransformedParameters::TransformIdSubseq trans_id,
                               const std::pair<int64_t, int64_t>& range);

    /**
     * @brief
     * @return
     */
    std::vector<ConfigSearchTranformedSeq>& getTransformedSeqs();

    /**
     * @brief
     * @return
     */
    bool increment();

    /**
     * @brief
     * @param rd
     * @param d
     */
    void mutate(std::mt19937& rd, std::normal_distribution<>& d);
};

/**
 * @brief
 */
class ConfigSearch {
 private:
    SearchSpace<int8_t> transformation;              //!< @brief
    size_t transformation_search_idx;                //!< @brief
    std::vector<ConfigSearchTransformation> params;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getTransform() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint32_t getTransformParam() const;

    /**
     * @brief
     * @return
     */
    std::vector<ConfigSearchTranformedSeq>& getTransformedSeqs();

    /**
     * @brief
     * @param range
     */
    explicit ConfigSearch(const std::pair<int64_t, int64_t>& range);

    /**
     * @brief
     * @return
     */
    bool increment();

    /**
     * @brief
     * @param rate
     */
    void mutate(float rate);

    /**
     * @brief
     * @param descriptor_subsequence
     * @param tokentype
     * @return
     */
    paramcabac::Subsequence createConfig(genie::core::GenSubIndex descriptor_subsequence, bool tokentype);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/gabac/benchmark.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BENCHMARK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

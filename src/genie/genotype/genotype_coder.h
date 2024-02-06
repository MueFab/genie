/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_GENOTYPE_CODER_H
#define GENIE_GENOTYPE_GENOTYPE_CODER_H

#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <sstream>
#include <tuple>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
// #include <xtensor/xmath.hpp>
// #include <xtensor/xoperation.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genotype_parameters.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace genotype {

// -----------------------------------------------------------------------------

using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
using UInt8MatDtype = xt::xtensor<uint8_t, 2, xt::layout_type::row_major>;
using Int8MatDtype = xt::xtensor<int8_t, 2, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;

using VecShapeDtype = xt::xtensor<size_t, 1>::shape_type;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;
using AttrType = std::vector<uint8_t>;

// ---------------------------------------------------------------------------------------------------------------------

enum class SortingAlgoID : uint8_t {
    NO_SORTING = 0,
    RANDOM_SORT = 1,
    NEAREST_NEIGHBOR = 2,
    LIN_KERNIGHAN_HEURISTIC = 3,
    UNDEFINED = 4
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t block_size;
    BinarizationID binarization_ID;
    ConcatAxis concat_axis;
    bool transpose_mat = false;
    SortingAlgoID sort_row_method;
    SortingAlgoID sort_col_method;
    genie::core::AlgoID codec_ID;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    uint8_t max_ploidy = 0;
    bool dot_flag = false;
    bool na_flag = false;

    Int8MatDtype allele_mat;
    BinMatDtype phasing_mat;

    UIntVecDtype amax_vec;
    std::vector<BinMatDtype> allele_bin_mat_vect;
    std::vector<UIntVecDtype> allele_row_ids_vect;
    std::vector<UIntVecDtype> allele_col_ids_vect;

    UIntVecDtype phasing_row_ids;
    UIntVecDtype phasing_col_ids;

    std::map<std::string, core::record::annotation_parameter_set::AttributeData> attributeInfo;

    // <block_size, num_samples, format_count/array_length, type>
    std::map<std::string, std::vector<std::vector<std::vector<AttrType>>>> attributeData;

    likelihood::LikelihoodParameters likelihoodParameters;
    std::vector<uint32_t> likelihoodData;
};

// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters generate_genotype_parameters(const EncodingOptions& opt, const EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

uint8_t getNumBinMats(const EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void decompose(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::VariantGenotype>& recs);

// ---------------------------------------------------------------------------------------------------------------------

void transform_max_value(EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_bit_plane(EncodingBlock& block, ConcatAxis concat_axis);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_row_bin(EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_allele_mat(const EncodingOptions& opt, EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void sort_matrix(BinMatDtype& bin_mat, UIntVecDtype ids, uint8_t axis);

// ---------------------------------------------------------------------------------------------------------------------

void random_sort_bin_mat(BinMatDtype& bin_mat, UIntVecDtype& ids, uint8_t axis);

// ---------------------------------------------------------------------------------------------------------------------

void sort_bin_mat(BinMatDtype& bin_mat, UIntVecDtype& row_ids, UIntVecDtype& col_ids, SortingAlgoID sort_row_method,
                  SortingAlgoID sort_col_method);

// ---------------------------------------------------------------------------------------------------------------------

void sort_block(const EncodingOptions& opt, EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void invert_sorted_block(const GenotypeParameters& params, EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_to_bytes(BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len);

// ---------------------------------------------------------------------------------------------------------------------

void entropy_encode_bin_mat(BinMatDtype& bin_mat, genie::core::AlgoID codec_ID, std::vector<uint8_t> payload);

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_from_bytes(BinMatDtype& bin_mat, const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols);

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia,Stefanie): Move and refactor this function to the parsing function
void sort_format(const std::vector<core::record::VariantGenotype>& recs, size_t block_size,
                 std::map<std::string, core::record::annotation_parameter_set::AttributeData>& info,
                 std::map<std::string, std::vector<std::vector<std::vector<AttrType>>>>& values);

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<GenotypeParameters, EncodingBlock> encode_block(const EncodingOptions& opt,
                                                           std::vector<core::record::VariantGenotype>& recs);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_GENOTYPE_CODER_H

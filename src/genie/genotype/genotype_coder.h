/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_GENOTYPE_CODER_H
#define GENIE_GENOTYPE_GENOTYPE_CODER_H

#include <cstdint>
#include <list>
#include <tuple>
#include <optional>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
//#include <xtensor/xmath.hpp>
//#include <xtensor/xoperation.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genotype_parameters.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace genotype {

using IntMatDtype = xt::xtensor<int8_t , 2, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint8_t, 1, xt::layout_type::row_major>;

// ---------------------------------------------------------------------------------------------------------------------

enum class SortingAlgoID: uint8_t {
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
    SortingAlgoID sort_row_method;
    SortingAlgoID sort_col_method;
    genie::core::AlgoID codec_ID;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    uint8_t max_ploidy = 0;
    bool dot_flag = false;
    bool na_flag = false;

    IntMatDtype allele_mat;
    BinMatDtype phasing_mat;

    UIntVecDtype amax_vec;
    std::vector<BinMatDtype> allele_bin_mat_vect;
    std::vector<UIntVecDtype> allele_row_ids_vect;
    std::vector<UIntVecDtype> allele_col_ids_vect;

    UIntVecDtype phasing_row_ids;
    UIntVecDtype phasing_col_ids;
};

// ---------------------------------------------------------------------------------------------------------------------

void decompose(
    const EncodingOptions& opt,
    EncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs
);

// ---------------------------------------------------------------------------------------------------------------------

void transform_max_value(
    EncodingBlock& block
);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_bit_plane(
    EncodingBlock& block,
    ConcatAxis concat_axis
);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_row_bin(
    EncodingBlock& block
);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_allele_mat(
    const EncodingOptions& opt,
    EncodingBlock& block
);

// ---------------------------------------------------------------------------------------------------------------------

void sort_matrix(
    BinMatDtype& bin_mat,
    UIntVecDtype ids,
    uint8_t axis
);

// ---------------------------------------------------------------------------------------------------------------------

void random_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& ids,
    uint8_t axis
);

// ---------------------------------------------------------------------------------------------------------------------

void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
);

// ---------------------------------------------------------------------------------------------------------------------

void sort_block(
    const EncodingOptions& opt,
    EncodingBlock& block
);

// ---------------------------------------------------------------------------------------------------------------------

void encode(
    const EncodingOptions& opt,
    std::list<core::record::VariantGenotype>& recs
);

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genotype
} // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_GENOTYPE_CODER_H

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

//using AlleleTensorDtype = xt::xtensor<int8_t, 3, xt::layout_type::row_major>;
using IntMatDtype = xt::xtensor<int8_t , 2, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
using BinMatsDtype = xt::xtensor<bool, 3, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint8_t, 1, xt::layout_type::row_major>;

//using AlleleTensorDtype = xt::xarray<int8_t, xt::layout_type::row_major>;
//using UnsignedAlleleTensorDtype = xt::xarray<uint8_t, xt::layout_type::row_major>;
//using BinMatDtype = xt::xarray<bool, xt::layout_type::row_major>;

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t block_size;
    BinarizationID binarization_ID;
    ConcatAxis concat_axis;
    bool sort_rows;
    bool sort_cols;
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
    BinMatsDtype bin_allele_mats;
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
    const EncodingOptions& opt,
    EncodingBlock& block
);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_row_bin(
    const EncodingOptions& opt,
    EncodingBlock& block
);

// ---------------------------------------------------------------------------------------------------------------------

void binarize_allele_mat(
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

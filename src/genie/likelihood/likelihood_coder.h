/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H
#define GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

#include <cmath>
#include <tuple>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
#include "genie/core/record/variant_genotype/record.h"
// #include <xtensor/xstrided_view.hpp>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

using UInt32ArrDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;
//using FP32MatDtype = xt::xtensor<uint32_t, 2, xt::layout_type::row_major>;
//using UInt8MatDtype = xt::xtensor<uint8_t, 2, xt::layout_type::row_major>;
//using UInt16MatDtype = xt::xtensor<uint16_t, 2, xt::layout_type::row_major>;
using UInt32MatDtype = xt::xtensor<uint32_t, 2, xt::layout_type::row_major>;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t block_size;
    bool transform_flag;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    uint32_t nrows;
    uint32_t ncols;
    UInt32MatDtype likelihood_mat;
    UInt32ArrDtype lut;
    uint32_t nelems;
    UInt32MatDtype idx_mat;
    core::DataType dtype_id = core::DataType::UINT32;
    std::stringstream serialized_mat;
    std::stringstream serialized_arr;
};

// ---------------------------------------------------------------------------------------------------------------------

void extract_likelihoods(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::VariantGenotype>& recs);

// ---------------------------------------------------------------------------------------------------------------------

void transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

void transform_lut(
    UInt32MatDtype& likelihood_mat,
    UInt32ArrDtype& lut,
    uint32_t& nelems,
    UInt32MatDtype& idx_mat,
    core::DataType& dtype_id
);

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat);

// ---------------------------------------------------------------------------------------------------------------------

void serialize_mat(
    UInt32MatDtype mat,
    core::DataType dtype_id,
    uint32_t& nrows,
    uint32_t& ncols,
    std::stringstream& payload
);

// ---------------------------------------------------------------------------------------------------------------------

void serialize_arr(
    UInt32ArrDtype arr,
    uint32_t nelems,
    std::stringstream& payload
);

// ---------------------------------------------------------------------------------------------------------------------

} // namespace likelihood {
} // namespace genie {

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

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

using FP32MatDtype = xt::xtensor<uint32_t, 2, xt::layout_type::row_major>;
using UInt16MatDtype = xt::xtensor<uint16_t, 2, xt::layout_type::row_major>;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t block_size;
    uint8_t transform_mode;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    FP32MatDtype likelihood_mat;
    UInt16MatDtype index_mat;
};

// ---------------------------------------------------------------------------------------------------------------------

void extract_likelihoods(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::VariantGenotype>& recs);

// ---------------------------------------------------------------------------------------------------------------------

void create_lut(const EncodingOptions& opt, EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

} // namespace likelihood {
} // namespace genie {

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

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

using SignedAlleleTensorDtype = xt::xtensor<int8_t, 3, xt::layout_type::row_major>;
using UnsignedAlleleTensorDtype = xt::xtensor<uint8_t, 3, xt::layout_type::row_major>;
using PhasingTensorDtype = xt::xtensor<bool, 3, xt::layout_type::row_major>;

//using SignedAlleleTensorDtype = xt::xarray<int8_t, xt::layout_type::row_major>;
//using UnsignedAlleleTensorDtype = xt::xarray<uint8_t, xt::layout_type::row_major>;
//using PhasingTensorDtype = xt::xarray<bool, xt::layout_type::row_major>;

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
//    uint32_t num_samples;
//    uint8_t max_ploidy;

    std::list<core::record::VariantGenotype> recs;
};

// ---------------------------------------------------------------------------------------------------------------------

void decompose(
    const EncodingOptions& opt,
   std::vector<core::record::VariantGenotype>& recs,
   SignedAlleleTensorDtype& signed_allele_tensor,
   PhasingTensorDtype& phasing_tensor);

//std::tuple<SignedAlleleTensorDtype, PhasingTensorDtype>&& decompose(
//    const EncodingOptions& opt,
//    std::vector<core::record::VariantGenotype>& recs
//);

// ---------------------------------------------------------------------------------------------------------------------

void encode(const EncodingOptions& opt, std::list<core::record::VariantGenotype>& recs);

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genotype
} // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_GENOTYPE_CODER_H

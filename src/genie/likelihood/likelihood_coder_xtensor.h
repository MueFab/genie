/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_LIKELIHOOD_CODER_XTENSOR_H
#define GENIE_LIKELIHOOD_LIKELIHOOD_CODER_XTENSOR_H

#include <vector>
#include <sstream>
#include <xtensor/xtensor.hpp>
#include "likelihood_types.h"
#include "genie/core/record/variant/record.h"
#include "likelihood_parameters.h"

namespace genie::likelihood {
    struct EncodingOptions;
    class LikelihoodPayload;
}

namespace genie::likelihood::detail {

struct LikelihoodEncodingBlock {
    uint32_t nrows;
    uint32_t ncols;
    std::stringstream serialized_mat;
    std::stringstream serialized_arr;
    
    // XTensor specific members
    xt::xtensor<uint32_t, 2> likelihood_mat;
    xt::xtensor<uint32_t, 1> lut;
    xt::xtensor<uint32_t, 2> idx_mat;
    
    uint32_t nelems;
    core::DataType dtype_id = core::DataType::UINT32;

    LikelihoodEncodingBlock() : nrows(0), ncols(0), nelems(0) {}
};

namespace xtensor {

void extract_likelihoods(
    const EncodingOptions& opt,
    LikelihoodEncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs);

void transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

void inverse_transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

void transform_lut(xt::xtensor<uint32_t, 2>& likelihood_mat, xt::xtensor<uint32_t, 1>& lut, uint32_t& nelems, xt::xtensor<uint32_t, 2>& idx_mat,
                   core::DataType& dtype_id);

void inverse_transform_lut(xt::xtensor<uint32_t, 2>& likelihood_mat, xt::xtensor<uint32_t, 1>& lut, xt::xtensor<uint32_t, 2>& idx_mat);

void serialize_mat(const xt::xtensor<uint32_t, 2>& mat, core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload);

void serialize_arr(const xt::xtensor<uint32_t, 1>& arr, uint32_t nelems, std::stringstream& payload);

void encode_likelihood(
    std::vector<core::record::VariantGenotype>& recs,
    LikelihoodParameters& params, LikelihoodPayload& payload,
    size_t block_size, bool transform_flag);

void decode_likelihood(
    const LikelihoodParameters& params, LikelihoodPayload& payload,
    std::vector<core::record::VariantGenotype>& recs);

} // namespace xtensor
} // namespace genie::likelihood::detail

#endif // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_XTENSOR_H

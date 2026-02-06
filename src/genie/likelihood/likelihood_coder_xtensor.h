/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_LIKELIHOOD_CODER_XTENSOR_H
#define GENIE_LIKELIHOOD_LIKELIHOOD_CODER_XTENSOR_H

#include <vector>
#include <sstream>
#include "likelihood_types.h"
#include "genie/core/variant_genotype_record/record.h"
#include "likelihood_parameters.h"

namespace genie::likelihood {
    struct EncodingOptions;
    struct EncodingBlock;
    class LikelihoodPayload;
}

namespace genie::likelihood::detail::xtensor {

void extract_likelihoods(
    const EncodingOptions& opt,
    EncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs);

void transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block);

void inverse_transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block);

void transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, uint32_t& nelems, UInt32MatDtype& idx_mat,
                   core::DataType& dtype_id);

void inverse_transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat);

void serialize_mat(UInt32MatDtype mat, core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload);

void serialize_arr(UInt32ArrDtype arr, uint32_t nelems, std::stringstream& payload);

void encode_likelihood(
    std::vector<core::record::VariantGenotype>& recs,
    LikelihoodParameters& params, LikelihoodPayload& payload,
    size_t block_size, bool transform_flag);

void decode_likelihood(
    const LikelihoodParameters& params, LikelihoodPayload& payload,
    std::vector<core::record::VariantGenotype>& recs);

} // namespace genie::likelihood::detail::xtensor

#endif // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_XTENSOR_H

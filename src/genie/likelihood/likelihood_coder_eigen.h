/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_LIKELIHOOD_LIKELIHOOD_CODER_EIGEN_H_
#define SRC_GENIE_LIKELIHOOD_LIKELIHOOD_CODER_EIGEN_H_

#include <sstream>
#include <vector>
#include "genie/likelihood/likelihood_types.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/core/record/variant/record.h"

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

    // STD specific members
    UInt32MatDtype likelihood_mat;
    UInt32ArrDtype lut;
    UInt32MatDtype idx_mat;

    uint32_t nelems;
    core::DataType dtype_id = core::DataType::UINT32;

    LikelihoodEncodingBlock() : nrows(0), ncols(0), nelems(0) {}
};
}  // namespace genie::likelihood::detail

namespace genie::likelihood::detail::eigen {

void extract_likelihoods(
    const EncodingOptions& opt,
    LikelihoodEncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs);

void transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

void inverse_transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

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

}  // namespace genie::likelihood::detail::eigen

#endif  // SRC_GENIE_LIKELIHOOD_LIKELIHOOD_CODER_EIGEN_H_

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/likelihood/likelihood_coder_eigen.h"
#include "genie/util/runtime_exception.h"

namespace genie::likelihood::detail::eigen {

void extract_likelihoods(const EncodingOptions&, EncodingBlock&, std::vector<core::record::VariantGenotype>&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void transform_likelihood_mat(const EncodingOptions&, EncodingBlock&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void inverse_transform_likelihood_mat(const EncodingOptions&, EncodingBlock&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void transform_lut(UInt32MatDtype&, UInt32ArrDtype&, uint32_t&, UInt32MatDtype&, core::DataType&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void inverse_transform_lut(UInt32MatDtype&, UInt32ArrDtype&, UInt32MatDtype&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void serialize_mat(UInt32MatDtype, core::DataType, uint32_t&, uint32_t&, std::stringstream&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void serialize_arr(UInt32ArrDtype, uint32_t, std::stringstream&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void encode_likelihood(std::vector<core::record::VariantGenotype>&, LikelihoodParameters&, LikelihoodPayload&, size_t, bool) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

void decode_likelihood(const LikelihoodParameters&, LikelihoodPayload&, std::vector<core::record::VariantGenotype>&) {
    UTILS_DIE("Likelihood Eigen backend not implemented yet");
}

} // namespace detail::eigen

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <xtensor/xsort.hpp>
#include "genie/util/runtime-exception.h"
#include "likelihood_coder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

void extract_likelihoods(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::VariantGenotype>& recs){
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ? opt.block_size : recs.size();
    uint32_t num_samples = recs.front().getNumSamples();
    uint8_t num_likelihoods = recs.front().getNumberOfLikelihoods();

    auto& likelihood_mat = block.likelihood_mat;

    MatShapeDtype likelihood_mat_shape = {block_size, num_samples * num_likelihoods};
    likelihood_mat = xt::empty<uint32_t , xt::layout_type::row_major>(likelihood_mat_shape);

    for (uint32_t i_rec = 0; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];

        UTILS_DIE_IF(num_samples != rec.getNumSamples(), "Number of samples is not constant within a block!");
        UTILS_DIE_IF(num_likelihoods != rec.getNumberOfLikelihoods(), "Number of likelihoods is not constant within a block!");

        auto& rec_likelihoods = rec.getLikelihoods();
        for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
            for (uint8_t k_likelihood = 0; k_likelihood < num_likelihoods; k_likelihood++) {
                likelihood_mat(i_rec, j_sample * num_likelihoods + k_likelihood) = rec_likelihoods[j_sample][k_likelihood];
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block){
    if (opt.transform_mode){
        transform_lut(block.likelihood_mat, block.lut, block.idx_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block){
    if (opt.transform_mode){
        inverse_transform_lut(block.likelihood_mat, block.lut, block.idx_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_lut(
    UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut,
    UInt32MatDtype& idx_mat
){
    auto m = likelihood_mat.shape(0);
    auto n = likelihood_mat.shape(1);

    idx_mat = xt::xtensor<uint32_t, 2>({m, n});
    lut = xt::unique(likelihood_mat);

    for (size_t i=0; i<m; i++){
        for (size_t j=0; j<n; j++){
            auto& likelihood_val = likelihood_mat(i, j);

            // Binary Search Algorithm
            uint32_t low = 0;
            uint32_t high = static_cast<uint32_t>(lut.shape(0)) - 1;

            while (low <= high) {
                uint32_t idx = (low + high) / 2;
                if (lut[idx] > likelihood_val)
                    high = idx - 1;
                else if (lut[idx] < likelihood_val)
                    low = idx + 1;
                else{
                    idx_mat(i, j) = idx;
                    break;
                }
            }
        }
    }

}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_lut(
    UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat
){

    likelihood_mat = xt::empty_like(idx_mat);
    auto m = likelihood_mat.shape(0);
    auto n = likelihood_mat.shape(1);

    for (size_t i=0; i<m; i++){
        for (size_t j=0; j<n; j++){
            likelihood_mat(i, j) = lut(idx_mat(i, j));
        }
    }

}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
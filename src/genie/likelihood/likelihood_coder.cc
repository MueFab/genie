/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <xtensor/xsort.hpp>
#include "genie/util/runtime-exception.h"
#include "likelihood_coder.h"

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

void create_lut(const EncodingOptions& opt, EncodingBlock& block){
//    auto out = xt::unique(block);

    auto x = 10;

}

// -----------------------------------------------------------------------------

}
}
// -----------------------------------------------------------------------------
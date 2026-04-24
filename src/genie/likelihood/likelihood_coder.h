/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H
#define GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

#include <vector>
#include <sstream>
#include <memory>
#include "genie/core/record/variant/record.h"
#include "likelihood_parameters.h"
#include "likelihood_types.h"

namespace genie::likelihood {

class LikelihoodPayload;

struct EncodingOptions {
    uint32_t block_size;
    bool transform_flag;
};

// Compatibility alias
using LikelihoodEncodingBlock = detail::LikelihoodEncodingBlock;

// Public API
void encode_likelihood(
    std::vector<core::record::VariantGenotype>& recs,
    LikelihoodParameters& params, LikelihoodPayload& payload,
    size_t block_size = 512, bool transform_flag = true);

void decode_likelihood(
    const LikelihoodParameters& params, LikelihoodPayload& payload,
    std::vector<core::record::VariantGenotype>& recs);

// Helper declarations
void extract_likelihoods(
    const EncodingOptions& opt,
    LikelihoodEncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs);

void transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);
void inverse_transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

} // namespace genie::likelihood

#endif // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_GENOTYPE_CODER_H
#define GENIE_GENOTYPE_GENOTYPE_CODER_H

#include <cstdint>
#include <list>
#include "genie/core/record/variant_genotype/record.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {

};

struct EncodingBlock {
    uint32_t num_samples;
    uint8_t max_ploidy;
    std::list<core::record::VariantGenotype> recs;
};

// ---------------------------------------------------------------------------------------------------------------------

void encode(const EncodingOptions& opt, const EncodingBlock& b);

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genotype
} // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_GENOTYPE_CODER_H

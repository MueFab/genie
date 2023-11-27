/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "likelihood_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodParameters::LikelihoodParameters(): num_gl_per_sample(0), transform_flag(false), dtype_id(0) {}

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodParameters::LikelihoodParameters(uint8_t _num_gl_per_sample, bool _transform_flag, uint8_t _dtype_id)
    : num_gl_per_sample(_num_gl_per_sample),
      transform_flag(_transform_flag),
      dtype_id(_dtype_id) {}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LikelihoodParameters::getNumGlPerSample() const { return num_gl_per_sample; }

// ---------------------------------------------------------------------------------------------------------------------

bool LikelihoodParameters::getTransformFlag() const { return transform_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LikelihoodParameters::getDtypeID() const { return dtype_id; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace likelihood
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
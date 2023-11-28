/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_PARAMETERS_H
#define GENIE_LIKELIHOOD_PARAMETERS_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

class LikelihoodParameters {
 private:
    uint8_t num_gl_per_sample;
    bool transform_flag;
    core::DataType dtype_id;

 public:
    LikelihoodParameters();
    LikelihoodParameters(uint8_t _num_gl_per_sample, bool _transform_flag, core::DataType _dtype_id);

    uint8_t getNumGlPerSample() const;
    bool getTransformFlag() const;
    core::DataType getDtypeID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace likelihood
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_LIKELIHOOD_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------

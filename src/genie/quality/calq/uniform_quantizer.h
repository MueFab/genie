/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_UNIFORM_QUANTIZER_H_
#define SRC_GENIE_QUALITY_CALQ_UNIFORM_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include "genie/quality/calq/quantizer.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

class UniformQuantizer : public Quantizer {
 public:
    UniformQuantizer(const int& valueMin, const int& valueMax, const int& nrSteps);
    ~UniformQuantizer() override;
};

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_UNIFORM_QUANTIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

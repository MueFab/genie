#ifndef SRC_GENIE_QUALITY_CALQ_UNIFORM_MIN_MAX_QUANTIZER_H_
#define SRC_GENIE_QUALITY_CALQ_UNIFORM_MIN_MAX_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include "uniform_quantizer.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

class UniformMinMaxQuantizer : public UniformQuantizer {
 public:
    UniformMinMaxQuantizer(const int& valueMin, const int& valueMax, const int& nrSteps);

    ~UniformMinMaxQuantizer() override;
};

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_UNIFORM_MIN_MAX_QUANTIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

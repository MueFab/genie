#ifndef SRC_GENIE_QUALITY_CALQ_UNIFORM_QUANTIZER_H_
#define SRC_GENIE_QUALITY_CALQ_UNIFORM_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include "quantizer.h"

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

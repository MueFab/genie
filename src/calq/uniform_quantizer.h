#ifndef CALQ_UNIFORM_QUANTIZER_H_
#define CALQ_UNIFORM_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include "calq/quantizer.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

class UniformQuantizer : public Quantizer
{
 public:
    UniformQuantizer(const int& valueMin,
                     const int& valueMax,
                     const int& nrSteps
    );
    ~UniformQuantizer() override;
};

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------

#endif  // CALQ_UNIFORM_QUANTIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
/** @file UniformMinMaxQuantizer.h
 *  @brief This file contains the definitions of the UniformMinMaxQuantizer
 *         class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_QUALCODEC_QUANTIZERS_UNIFORMMINMAXQUANTIZER_H_
#define CALQ_QUALCODEC_QUANTIZERS_UNIFORMMINMAXQUANTIZER_H_

#include "QualCodec/Quantizers/UniformQuantizer.h"

namespace calq {

class UniformMinMaxQuantizer : public UniformQuantizer {
 public:
    UniformMinMaxQuantizer(const int &valueMin, const int &valueMax, const int &nrSteps);
    ~UniformMinMaxQuantizer(void);
};

}  // namespace calq

#endif  // CALQ_QUALCODEC_QUANTIZERS_UNIFORMMINMAXQUANTIZER_H_


/** @file UniformQuantizer.h
 *  @brief This file contains the definitions of the UniformQuantizer class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_QUALCODEC_QUANTIZERS_UNIFORMQUANTIZER_H_
#define CALQ_QUALCODEC_QUANTIZERS_UNIFORMQUANTIZER_H_

#include "QualCodec/Quantizers/Quantizer.h"

namespace calq {

class UniformQuantizer : public Quantizer {
 public:
    UniformQuantizer(const int &valueMin, const int &valueMax, const int &nrSteps);
    ~UniformQuantizer(void);
};

}  // namespace calq

#endif  // CALQ_QUALCODEC_QUANTIZERS_UNIFORMQUANTIZER_H_


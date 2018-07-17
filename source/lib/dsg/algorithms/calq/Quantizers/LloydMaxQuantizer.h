/** @file LloydMaxQuantizer.h
 *  @brief This file contains the definition of a Lloyd quantizer
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#ifndef CALQ_QUALCODEC_QUANTIZERS_LLOYDMAXQUANTIZER_H_
#define CALQ_QUALCODEC_QUANTIZERS_LLOYDMAXQUANTIZER_H_

#include <math.h>

#include <vector>

#include "QualCodec/Quantizers/Quantizer.h"
#include "Common/Exceptions.h"

// ----------------------------------------------------------------------------------------------------------------------

namespace calq {

class LloydMaxQuantizer : public Quantizer{
 private:
    std::vector<size_t> pdf;
    size_t rangeMin;

    std::vector<double> borders;
    std::vector<double> values;

    size_t steps;

    void fillLUT();

    double calcCentroid(size_t left, size_t right);

    void calcBorders();

 public:
    LloydMaxQuantizer(size_t rangeMin, size_t rangeMax, size_t steps);

    void addToPdf(size_t qualScore, size_t number = 1);

    void resetPdf();

    void build();

    std::vector<double> exportValues();

    template<typename T>
    void importValues(const std::vector<T>& imp) {
        if (imp.size() != steps) {
            throwErrorException("Number of values does not match quantizer size");
        }

        values.clear();

        for (T val : imp) {
            values.push_back(static_cast<double>(val));
        }
    }
};
}  // namespace calq

// ----------------------------------------------------------------------------------------------------------------------

#endif  // CALQ_QUALCODEC_QUANTIZERS_LLOYDMAXQUANTIZER_H_

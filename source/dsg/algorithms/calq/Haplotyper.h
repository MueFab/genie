/** @file Haplotyper.h
 *  @brief This file contains the definition of the Haplotyper class.
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#ifndef CALQ_QUALCODEC_HAPLOTYPER_H_
#define CALQ_QUALCODEC_HAPLOTYPER_H_

#include <functional>
#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "FilterBuffer.h"
#include "Genotyper.h"
#include "SoftclipSpreader.h"

// ----------------------------------------------------------------------------------------------------------------------

namespace calq {

class Haplotyper {
 private:
    const size_t SIGMA;

    // Saving kernel and old raw quality scores
    FilterBuffer buffer;

    GaussKernel kernel;
    SoftclipSpreader spreader;
    calq::Genotyper genotyper;
    size_t nr_quantizers;
    size_t polyploidy;

    double NO_INDEL_LIKELIHOOD;
    double INDEL_LIKELIHOOD;

 public:
    // Init
    Haplotyper(size_t sigma, size_t ploidy, size_t qualOffset, size_t nrQuantizers, size_t maxHQSoftclip_propagation,
               size_t minHQSoftclip_streak, size_t gaussRadius);

    // Returns offset between activity scores' position and front
    size_t getOffset() const;

    // Pushes new activity score calculated using parameters and returns filtered acticityscore for (pos-offset)
    size_t push(const std::string& seqPile, const std::string& qualPile, size_t hq_softclips, char reference);

    std::vector<double> calcNonRefLikelihoods(char ref, const std::string& seqPile, const std::string& qualPile);

    double calcActivityScore(char ref, const std::string& seqPile, const std::string& qualPile, double heterozygosity);

    std::vector<double> calcPriors(double hetero);
};
}  // namespace calq

// ----------------------------------------------------------------------------------------------------------------------

#endif  // CALQ_QUALCODEC_HAPLOTYPER_H_

// ----------------------------------------------------------------------------------------------------------------------

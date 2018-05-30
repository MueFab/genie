/** @file Haplotyper.cc
 *  @brief This file contains the implementation of the Haplotyper class.
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#include "QualCodec/Haplotyper.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <algorithm>
#include <utility>

#include <cmath>

// ----------------------------------------------------------------------------------------------------------------------

namespace calq {

// Returns score, takes seq and qual pileup and position
Haplotyper::Haplotyper(size_t sigma, size_t ploidy, size_t qualOffset, size_t nrQuantizers, size_t maxHQSoftclip_propagation,
                       size_t minHQSoftclip_streak, size_t gaussRadius)
                       : SIGMA(sigma), kernel(sigma), spreader(maxHQSoftclip_propagation, minHQSoftclip_streak, true),
                         genotyper(ploidy, qualOffset, nrQuantizers), nr_quantizers(nrQuantizers), polyploidy(ploidy) {
    double THRESHOLD = 0.0000001;
    size_t size = this->kernel.calcMinSize(THRESHOLD, gaussRadius*2+1);

    buffer = FilterBuffer([this](size_t pos, size_t size) -> double{return this->kernel.calcValue(pos, size);}, size);

    NO_INDEL_LIKELIHOOD = log10(1.0-pow(10, -4.5));
    INDEL_LIKELIHOOD = -4.5;
}

// ----------------------------------------------------------------------------------------------------------------------

size_t Haplotyper::getOffset() const {
    return buffer.getOffset() + spreader.getOffset() - 1;
}

// ----------------------------------------------------------------------------------------------------------------------


double log10sum(double a, double b) {
    if (a > b) {
        log10sum(b, a);
    } else if (a == - std::numeric_limits<double>::infinity()) {
        return b;
    }
    return b + log10(1+ pow(10.0, -(b-a)));
}

// ----------------------------------------------------------------------------------------------------------------------

// Calc priors like in GATK
std::vector<double> Haplotyper::calcPriors(double hetero) {
    hetero = log10(hetero);
    std::vector<double> result(polyploidy+1, hetero);
    double sum = - std::numeric_limits<double>::infinity();
    for (size_t i = 1; i < polyploidy+1; ++i) {
        result[i] -= log10(i);
        sum = log10sum(sum, result[i]);
    }
    result[0] = log10(1.0 - exp10(sum));

    return result;
}

// ----------------------------------------------------------------------------------------------------------------------

std::vector<double> Haplotyper::calcNonRefLikelihoods(char ref, const std::string& seqPile, const std::string& qualPile) {
    std::vector<double> result(polyploidy+1, 0.0);
    std::map<std::string, double> SNPlikelihoods = genotyper.getGenotypelikelihoods(seqPile, qualPile);

    for (const std::pair<std::string, double> &m : SNPlikelihoods) {
        size_t altCount = polyploidy - std::count(m.first.begin(), m.first.end(), ref);
        result[altCount] += m.second;
    }

    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = log10(result[i]);
    }

    return result;
}

// ----------------------------------------------------------------------------------------------------------------------

double Haplotyper::calcActivityScore(char ref, const std::string& seqPile, const std::string& qualPile, double heterozygosity) {
    if (ref == 'N') {
        return 1.0;
    }
    std::vector<double> likelihoods = calcNonRefLikelihoods(ref, seqPile, qualPile);
    static std::vector<double> priors;
    if (priors.size() == 0) {
        priors = calcPriors(heterozygosity);
    }

    // --------------Calc Posteriors like in GATK ------------------
    double posteriori0 = likelihoods[0] + priors[0];
    bool map0 = true;
    for (size_t i = 1; i < polyploidy+1; ++i) {
        if (likelihoods[i] + priors[i] >  posteriori0) {
            map0 = false;
            break;
        }
    }

    if (map0) {
        return 0.0;
    }

    double altLikelihoodSum = - std::numeric_limits<double>::infinity();
    double altPriorSum = - std::numeric_limits<double>::infinity();

    for (size_t i = 1; i < polyploidy+1; ++i) {
        altLikelihoodSum = log10sum(altLikelihoodSum, likelihoods[i]);
        altPriorSum = log10sum(altPriorSum, priors[i]);
    }

    double altPosteriorSum = altLikelihoodSum + altPriorSum;
    // Normalize
    posteriori0 = posteriori0 - log10sum(altPosteriorSum, posteriori0);

    // --------------------------------------------------------

    const double CUTOFF = -1.0;

    if (posteriori0 > CUTOFF)
        return 0.0;

    return 1.0 - exp10(posteriori0);
}

// ----------------------------------------------------------------------------------------------------------------------

size_t Haplotyper::push(const std::string& seqPile, const std::string& qualPile, size_t hq_softclips, char reference) {
    // Empty input
    if (seqPile.empty()) {
        buffer.push(spreader.push(0.0, 0));
        return 0;
    }

    // Build reference string

    const double HETEROZYGOSITY = 1.0/1000.0;

    double altProb = calcActivityScore(reference, seqPile, qualPile, HETEROZYGOSITY);


    // Filter activity score
    buffer.push(spreader.push(altProb, hq_softclips/qualPile.size()));
    double activity = buffer.filter();

    size_t quant =  std::min(activity * nr_quantizers / 0.02, static_cast<double>(nr_quantizers-1));

#define CALQ_DEBUG 0
#if CALQ_DEBUG
    static CircularBuffer<std::string> debug(this->getOffset(), "\n");
    std::stringstream s;

    s << reference << " " << seqPile  << " ";

    s << std::fixed << std::setw(6) << std::setprecision(4)
      << std::setfill('0') << altProb;

    std::string out = debug.push(s.str());
    if (out != "\n") {
        std::cerr << out << " " << std::fixed << std::setw(6) << std::setprecision(4)
                  << std::setfill('0') << activity << " " << quant+2 << std::endl;
    }

    if (hq_softclips > 0)
        std::cerr << hq_softclips << " detected!" << std::endl;
#endif

    return quant;
}
}  // namespace calq

// ----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------


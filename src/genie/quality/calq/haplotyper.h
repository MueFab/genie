#ifndef CALQ_HAPLOTYPER_H_
#define CALQ_HAPLOTYPER_H_

#include <cmath>

// -----------------------------------------------------------------------------

#include <functional>
#include <map>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

#include "filter_buffer.h"
#include "genotyper.h"
#include "softclip_spreader.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

enum struct FilterType;

// -----------------------------------------------------------------------------

class Haplotyper
{
 private:
    // const size_t SIGMA;

    // Saving kernel and old raw quality scores
    FilterBuffer buffer;

    SoftclipSpreader spreader;
    calq::Genotyper genotyper;
    size_t nr_quantizers;
    size_t polyploidy;
    const bool DEBUG;

    const bool squashedActivity;
    double localDistortion;

    size_t getQuantizerIndex(double activity);

 public:
    // Init
    Haplotyper(size_t sigma,
               size_t ploidy,
               size_t qualOffset,
               size_t nrQuantizers,
               size_t maxHQSoftclip_propagation,
               size_t minHQSoftclip_streak,
               size_t filterCutOff,
               bool debug,
               bool squashed,
               FilterType filterType
    );

    // Returns offset between activity scores' position and front
    size_t getOffset() const;

    // Pushes new activity score calculated using parameters and
    // returns filtered acticityscore for (pos-offset)
    size_t push(const std::string& seqPile,
                const std::string& qualPile,
                size_t hq_softclips,
                char reference
    );

    std::vector<double> calcNonRefLikelihoods(char ref,
                                              const std::string& seqPile,
                                              const std::string& qualPile
    );

    double calcActivityScore(char ref,
                             const std::string& seqPile,
                             const std::string& qualPile,
                             double heterozygosity
    );

    std::vector<double> calcPriors(double hetero);
};

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------

#endif  // CALQ_HAPLOTYPER_H_

// -----------------------------------------------------------------------------

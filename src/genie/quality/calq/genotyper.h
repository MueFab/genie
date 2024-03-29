/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_GENOTYPER_H_
#define SRC_GENIE_QUALITY_CALQ_GENOTYPER_H_

// -----------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

class Genotyper {
 public:
    Genotyper(const int& polyploidy, const int& qualOffset, const int& nrQuantizers, bool debug);
    ~Genotyper();

    double computeEntropy(const std::string& seqPileup, const std::string& qualPileup);
    int computeQuantizerIndex(const std::string& seqPileup, const std::string& qualPileup);

    const std::map<std::string, double>& getGenotypelikelihoods(const std::string& seqPileup,
                                                                const std::string& qualPileup);

 private:
    void initLikelihoods();
    void resetLikelihoods();
    void computeGenotypeLikelihoods(const std::string& seqPileup, const std::string& qualPileup, const size_t& depth);

    const std::vector<char> ALLELE_ALPHABET = {'A', 'C', 'G', 'T'};
    static constexpr size_t ALLELE_ALPHABET_SIZE = 4;

    const std::vector<char> alleleAlphabet_;
    std::map<char, double> alleleLikelihoods_;
    std::vector<std::string> genotypeAlphabet_;
    std::map<std::string, double> genotypeLikelihoods_;
    const int nrQuantizers_;
    const int polyploidy_;
    const int qualOffset_;
    const bool DEBUG;
};

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_GENOTYPER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

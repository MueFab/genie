/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_coder.h"
#include <xtensor/xview.hpp>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

void decompose(
    const EncodingOptions& opt,
    std::vector<core::record::VariantGenotype>& recs,
    SignedAlleleTensorDtype& signed_allele_tensor,
    PhasingTensorDtype& phasing_tensor){

    UTILS_DIE_IF(recs.empty(),
                 "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ?  opt.block_size : recs.size();
    uint32_t num_samples = recs.front().getNumSamples();

    uint32_t i_rec = 0;
    uint8_t max_ploidy = 0;
    // Precompute max ploidy do avoid resizing during process
    for (i_rec = 0; i_rec < block_size; i_rec++){
        auto& rec = recs[i_rec];

        UTILS_DIE_IF(num_samples != rec.getNumSamples(),
                     "Number of samples is not constant within a block!");

        auto tmp_p = static_cast<uint8_t>(rec.getNumberOfAllelesPerSample());
        max_ploidy = max_ploidy > tmp_p ? max_ploidy : tmp_p;
    }

    signed_allele_tensor = SignedAlleleTensorDtype({block_size, num_samples, max_ploidy});
    xt::view(signed_allele_tensor, xt::all(), xt::all(), xt::all()) = -2; // Replacement of "full" operator

    phasing_tensor = PhasingTensorDtype({block_size, num_samples, static_cast<uint8_t>(max_ploidy -1)});

    for (i_rec = 0; i_rec < block_size; i_rec++){
        auto& rec = recs[i_rec];

        // Check and update num_samples;
        if (num_samples == 0){
            num_samples = rec.getNumSamples();
        } else {
            UTILS_DIE_IF(num_samples != rec.getNumSamples(),
                                      "Number of samples is not constant within a block!");
        }

        auto& rec_alleles = rec.getAlleles();
        for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++){
            for (uint8_t k_allele = 0; k_allele < rec.getNumberOfAllelesPerSample(); k_allele++){
                signed_allele_tensor(i_rec, j_sample, k_allele) = rec_alleles[j_sample][k_allele];
            }
        }

        if (max_ploidy-1 > 0) {
            auto& rec_phasings = rec.getPhasing();
            for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++){
                for (uint8_t k_allele = 0; k_allele < rec.getNumberOfAllelesPerSample()-1; k_allele++) {
                    phasing_tensor(i_rec, j_sample, k_allele) = rec_phasings[j_sample][k_allele];
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode(const EncodingOptions& opt, std::list<core::record::VariantGenotype>& recs){

}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genotype
} // namespace genie

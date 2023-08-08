/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_coder.h"
#include <cmath>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xstrided_view.hpp>
#include <xtensor/xview.hpp>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

void decompose(
    const EncodingOptions& opt,
    EncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs
//    AlleleTensorDtype& signed_allele_tensor,
//    PhasingTensorDtype& phasing_tensor
){

    UTILS_DIE_IF(recs.empty(),
                 "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ?  opt.block_size : recs.size();
    uint32_t num_samples = recs.front().getNumSamples();

    uint32_t i_rec = 0;
    auto& max_ploidy = block.max_ploidy;
    // Precompute max ploidy do avoid resizing during process
    for (i_rec = 0; i_rec < block_size; i_rec++){
        auto& rec = recs[i_rec];

        UTILS_DIE_IF(num_samples != rec.getNumSamples(),
                     "Number of samples is not constant within a block!");

        auto tmp_p = static_cast<uint8_t>(rec.getNumberOfAllelesPerSample());
        max_ploidy = max_ploidy > tmp_p ? max_ploidy : tmp_p;
    }

    auto& allele_mat = block.allele_mat;
    allele_mat = IntMatDtype({block_size, num_samples*max_ploidy});
    // Initialize value with "not available"
    xt::view(allele_mat, xt::all(), xt::all(), xt::all()) = -2;

    auto& phasing_mat = block.phasing_mat;
    phasing_mat = BinMatDtype({block_size, num_samples * static_cast<uint8_t>(max_ploidy -1)});

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
            // Iterate only until given number of allele of the current record
            for (uint8_t k_allele = 0; k_allele < rec.getNumberOfAllelesPerSample(); k_allele++){
                allele_mat(i_rec, j_sample*max_ploidy + k_allele) = static_cast<int8_t>(rec_alleles[j_sample][k_allele]);
            }
        }

        if (max_ploidy-1 > 0) {
            auto phasing_ploidy = max_ploidy-1;
            auto& rec_phasings = rec.getPhasing();
            for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++){
                // Iterate only until given number of allele of the current record
                for (uint8_t k_allele = 0; k_allele < rec.getNumberOfAllelesPerSample()-1; k_allele++) {
                    phasing_mat(i_rec, j_sample*phasing_ploidy + k_allele) = rec_phasings[j_sample][k_allele];
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_max_value(
    EncodingBlock& block
){
    auto& allele_mat = block.allele_mat;

    block.dot_flag = xt::any(xt::equal(allele_mat, -1));
    block.na_flag = xt::any(xt::equal(allele_mat, -2));
    auto max_val = xt::amax(allele_mat)(0);

    if (block.dot_flag){
        max_val += 1;
        xt::filter(allele_mat, xt::equal(allele_mat, -1)) = max_val;
    }

    if (block.na_flag){
        max_val += 1;
        xt::filter(allele_mat, xt::equal(allele_mat, -2)) = max_val;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_bit_plane(
    const EncodingOptions& opt,
    EncodingBlock& block
){
    auto& allele_mat = block.allele_mat;
    auto nrows = allele_mat.shape(0);
    auto ncols = allele_mat.shape(1);
    auto max_val = xt::amax(allele_mat)(0);

    auto num_bit_planes = static_cast<uint8_t>(ceil(log2(max_val + 1)));
    auto& bin_mats = block.bin_allele_mats;

    bin_mats = BinMatsDtype({num_bit_planes, nrows, ncols});

    for (uint8_t i_mat = 0; i_mat < num_bit_planes; i_mat++){
        xt::view(bin_mats, i_mat, xt::all(), xt::all()) = allele_mat & (1 << i_mat);
    }

    //TODO @Yeremia: Free memory of allele_mat
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_row_bin(
    const EncodingOptions& opt,
    EncodingBlock& block
){

    auto& allele_mat = block.allele_mat;
    auto& amax_vec = block.amax_vec;
    auto nrows = allele_mat.shape(0);
    auto ncols = allele_mat.shape(1);
    amax_vec = xt::cast<uint8_t>(
                    xt::ceil(
                        xt::log2(
                            xt::amax(allele_mat, 1) + 1
                        )
                    )
                );

    auto& bin_mats = block.bin_allele_mats;
    auto new_nrows = static_cast<size_t>(xt::sum(amax_vec)(0));

    bin_mats = BinMatsDtype ({1, new_nrows, ncols});

    size_t i2 = 0;
    for (size_t i = 0; i < nrows; i++){
        for (size_t i_bit = 0; i_bit < amax_vec[i]; i_bit++){
            xt::view(bin_mats, 0, i2++, xt::all()) = xt::view(allele_mat, i, xt::all()) & (1 << i_bit);
        }
    }

//    UTILS_DIE_IF(i2 != new_nrows, "Something wrong with the for-loop");
    //TODO @Yeremia: Free memory of allele_mat
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_allele_mat(
    const EncodingOptions& opt,
    EncodingBlock& block
){
    if (opt.binarization_ID == genie::genotype::BinarizationID::BIT_PLANE){
        binarize_bit_plane(opt, block);
    } else if (opt.binarization_ID == genie::genotype::BinarizationID::ROW_BIN){
        binarize_row_bin(opt, block);
    } else {
        UTILS_DIE("Invalid binarization_ID!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode(
    const EncodingOptions& opt,
    std::list<core::record::VariantGenotype>& recs
){

}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genotype
} // namespace genie

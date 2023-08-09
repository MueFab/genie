/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_coder.h"
#include <cmath>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xsort.hpp>
//#include <xtensor/xstrided_view.hpp>
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
    EncodingBlock& block,
    ConcatAxis concat_axis
){
    auto& allele_mat = block.allele_mat;
    auto max_val = xt::amax(allele_mat)(0);

    auto num_bit_planes = static_cast<uint8_t>(ceil(log2(max_val + 1)));
    auto& bin_mats = block.allele_bin_mat_vect;
    bin_mats.resize(num_bit_planes);

//    bin_mats = BinMatsDtype({num_bit_planes, nrows, ncols});

    for (uint8_t k = 0; k < num_bit_planes; k++){
        bin_mats[k] = allele_mat & (1 << k);
    }

    if (concat_axis != ConcatAxis::DO_NOT_CONCAT){
        for (uint8_t k = 1; k < num_bit_planes; k++){
            if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
                bin_mats[0] = xt::concatenate(xt::xtuple(bin_mats[0], bin_mats[k]), 0);
            } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR){
                bin_mats[0] = xt::concatenate(xt::xtuple(bin_mats[0], bin_mats[k]), 1);
            }
        }
        bin_mats.resize(1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_row_bin(
    EncodingBlock& block
){

    auto& allele_mat = block.allele_mat;
    auto& amax_vec = block.amax_vec;
    auto nrows = allele_mat.shape(0);
    auto ncols = allele_mat.shape(1);
    amax_vec = xt::cast<uint8_t>(xt::ceil(xt::log2(
                                    xt::amax(allele_mat, 1) + 1)));

    auto& bin_mats = block.allele_bin_mat_vect;
    bin_mats.resize(1);
    auto new_nrows = static_cast<size_t>(xt::sum(amax_vec)(0));

//    bin_mats = BinMatsDtype ({1, new_nrows, ncols});
    auto& bin_mat = bin_mats[0];
    bin_mat.resize({new_nrows, ncols});

    size_t i2 = 0;
    for (size_t i = 0; i < nrows; i++){
        for (size_t i_bit = 0; i_bit < amax_vec[i]; i_bit++){
            xt::view(bin_mat, i2++, xt::all()) = xt::view(allele_mat, i, xt::all()) & (1 << i_bit);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_allele_mat(
    const EncodingOptions& opt,
    EncodingBlock& block
){

    switch(opt.binarization_ID){
        case genie::genotype::BinarizationID::BIT_PLANE:
            binarize_bit_plane(block, opt.concat_axis);
            break;
        case genie::genotype::BinarizationID::ROW_BIN:
            binarize_row_bin(block);
            break;
        default:
            UTILS_DIE("Invalid binarization_ID!");
    }

    //TODO @Yeremia: Free memory of allele_mat
//    delete block.allele_mat;
    block.allele_mat.resize({0,0});
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_matrix(
    BinMatDtype& bin_mat,
    UIntVecDtype ids,
    uint8_t axis
){
    UTILS_DIE_IF(axis > 1, "Invalid axis value!");
    UTILS_DIE_IF(bin_mat.shape(axis) != ids.shape(0),
                 "bin_mat and ids have different dimension!");

    //TODO @Yeremia: create a boolean vector for the buffer instead of whole matrix;
    BinMatDtype tmp_bin_mat = xt::empty_like(bin_mat);
    if (axis == 0) {
        for (uint32_t i = 0; i < ids.shape(0); i++) {
            xt::view(tmp_bin_mat, i, xt::all()) = xt::view(tmp_bin_mat, ids[i], xt::all());
        }
    } else if (axis == 1) {
        for (uint32_t i = 0; i < ids.shape(0); i++) {
            xt::view(tmp_bin_mat, xt::all(), i) = xt::view(tmp_bin_mat, xt::all(), ids[i]);
        }
    }

    bin_mat = tmp_bin_mat;
}

// ---------------------------------------------------------------------------------------------------------------------

void random_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& ids,
    uint8_t axis
){
    UTILS_DIE_IF(axis > 1, "Invalid axis value!");

    auto num_elem = static_cast<uint32_t>(bin_mat.shape(axis));
    ids = xt::random::permutation(num_elem);
    sort_matrix(bin_mat, ids, axis);
    ids = xt::argsort(ids);
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
){
    switch(sort_row_method){
        case genie::genotype::SortingAlgoID::NO_SORTING:
            break;
        case genie::genotype::SortingAlgoID::RANDOM_SORT:
            random_sort_bin_mat(bin_mat, row_ids, 0);
            break;
        case genie::genotype::SortingAlgoID::NEAREST_NEIGHBOR:
            UTILS_DIE("Not yet implemented!");
            break;
        case genie::genotype::SortingAlgoID::LIN_KERNIGHAN_HEURISTIC:
            UTILS_DIE("Not yet implemented!");
            break;
        default:
            UTILS_DIE("Invalid sort method!");
    }

    switch(sort_col_method){
        case genie::genotype::SortingAlgoID::NO_SORTING:
            break;
        case genie::genotype::SortingAlgoID::RANDOM_SORT:
            random_sort_bin_mat(bin_mat, col_ids, 1);
            break;
        case genie::genotype::SortingAlgoID::NEAREST_NEIGHBOR:
            UTILS_DIE("Not yet implemented!");
            break;
        case genie::genotype::SortingAlgoID::LIN_KERNIGHAN_HEURISTIC:
            UTILS_DIE("Not yet implemented!");
            break;
        default:
            UTILS_DIE("Invalid sort method!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_block(
    const EncodingOptions& opt,
    EncodingBlock& block
){
    auto num_bin_mats = static_cast<uint8_t>(block.allele_bin_mat_vect.size());

    // Sort (boolean) alelle matrices
    block.allele_row_ids_vect.resize(num_bin_mats);
    block.allele_col_ids_vect.resize(num_bin_mats);
    for (uint8_t i_mat = 0; i_mat < num_bin_mats; i_mat++){
        sort_bin_mat(
            block.allele_bin_mat_vect[i_mat],
            block.allele_row_ids_vect[i_mat],
            block.allele_col_ids_vect[i_mat],
            opt.sort_row_method,
            opt.sort_col_method
        );
    }

    // Sort phasing matrix
    sort_bin_mat(
        block.phasing_mat,
        block.phasing_row_ids,
        block.phasing_col_ids,
        opt.sort_row_method,
        opt.sort_col_method
    );
}

// ---------------------------------------------------------------------------------------------------------------------

//void encode(
//    const EncodingOptions& opt,
//    std::list<core::record::VariantGenotype>& recs
//){
//
//}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genotype
} // namespace genie

